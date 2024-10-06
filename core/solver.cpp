#include "solver.h"

#include "tubemodel.h"
#include "boardmodel.h"
#include "moveitem.h"

SolveProcess::SolveProcess(BoardModel *startBoard)
{
    m_startBoard = new BoardModel();
    m_moves = new MoveItems();
    m_hashes = new QVector<quint32>;
    if (startBoard)
        setStartBoard(startBoard);
}

SolveProcess::~SolveProcess()
{
    clear();
    delete m_hashes;
    delete m_moves;
    delete m_startBoard;
}

void SolveProcess::clear()
{
    m_moves->clear();
    m_hashes->clear();
    m_startBoard->clear();
}

void SolveProcess::setStartBoard(BoardModel *startBoard)
{
    if (startBoard) {
        m_startBoard->clear();
        m_startBoard->addTubesFrom(*startBoard);
        m_startBoard->calculateHash();
        m_startBoard->findMoves();
    }
}

void SolveProcess::stop()
{
    m_externalBreak = true;
}

void SolveProcess::start(BoardModel *startBoard)
{
    if (startBoard)
        setStartBoard(startBoard);

    // initial checks: if the board is empty
    if (m_startBoard->tubesCount() == 0) {
        qDebug() << "Start board is not defined. Nothing to solve.";
        m_resultId = CT_SOLVER_NO_BOARD;
        emit finished(m_resultId);
        return;
    }

    // initial checks: if the board has any available moves
    if (!m_startBoard->hasMoves()) {
        qDebug() << "Start board has no any moves. Solve process cannot be started.";
        m_resultId = CT_SOLVER_NO_MOVES;
        emit finished(m_resultId);
        return;
    }

//    qDebug() << "Start solving with" << m_startBoard;

    // preparing to start
    m_moves->clear();
    m_hashes->clear();
    m_externalBreak = false;

    bool solved = false;
    MoveItem * move = m_startBoard->currentMove();
    int  boardsCounter = 1; // Start board is already counted
    int  movesCounter = m_startBoard->movesCount();

    // doing solve
    do {

        // a move is considered successful if after it:
        //  - the game is solved, or
        //  - there are more moves on the resulting board
        bool moveSuccess = move->doMove();
        boardsCounter ++;

        // additional check of the resulting board,
        // if a similar game combination has occurred before
        if (moveSuccess) {
            moveSuccess = !(m_hashes->contains( move->boardAfter()->hash() ));
            movesCounter += move->boardAfter()->movesCount();
        }

        if (moveSuccess) {

            solved = move->boardAfter()->isSolved();
            if (!solved) {
                // adds board's hash to the stack
                m_hashes->append(move->boardAfter()->hash());
                // goes on with the highest ranked move on the new board
                move = move->boardAfter()->currentMove();
            }

        } else { // move is unsuccessful

            // uses the same board
            BoardModel * board = move->boardBefore();

            do {
                // the highest ranked move failed, so remove it
                board->removeCurrentMove();
                // and goes with the next ranked move
                move = board->currentMove();

                // if this board's moves are over
                if (!move)
                    // goes with the parent board
                    board = board->parentBoard();

            } while (board && !move);
        }

    } while (!m_externalBreak && !solved && move);

/*
      qDebug().nospace() << "== Solve process is finished ==\n"
             << "solved: " << solved
             << " | external break: " << m_externalBreak
             << " | boards counted: " << boardsCounter
             << " | moves counted: " << movesCounter
             << " | unique boards: " << m_hashes->size();
*/

    if (solved) {

        qDebug() << "The board is solved!";

//        if (move)
//            qDebug() << "The final board is" << move->boardAfter();

        // fill moves vector
        while (move) {

            MoveData moveData;
            moveData.stored = move->stored();

            // checks if in the next move this color moves to another tube again.
            // the next move is already stored in m_moves and now is the first one.
            if ( !m_moves->isEmpty()
                    && (moveData.fields.color  == m_moves->first()->color())
                    && (moveData.fields.count  == m_moves->first()->count())
                    && (moveData.fields.tubeTo == m_moves->first()->tubeFrom()))
            {
                moveData.fields.tubeTo = m_moves->first()->tubeTo();
                m_moves->removeFirst();
            }

            m_moves->prepend(new MoveItem(moveData.stored));
            move = move->parent();
        }

/*
        qDebug() << "Moves to solve:";
        for (int i = 0; i < m_moves->size(); ++i)
            qDebug().nospace() << "Move #" << i << " - " << m_moves->at(i);
*/

        m_resultId = CT_SOLVER_SUCCESS;

    } else { // the game is not solved

        if (m_externalBreak) {
            qDebug() << "You have cancelled your search for a solution to the game.";
            m_resultId = CT_SOLVER_CANCELLED;
        } else {
            qDebug() << "The game solution is not found. Try to undo some previous moves.";
            m_resultId = CT_SOLVER_NOT_FOUND;
        }
    }

    // clear memory excluding StartBoard and its own moves
    m_hashes->clear();
    m_startBoard->removeChildrenMoves();

    emit finished(m_resultId);
}
