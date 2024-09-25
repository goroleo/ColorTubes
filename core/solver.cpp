#include "solver.h"

#include "tubemodel.h"
#include "boardmodel.h"
#include "moveitem.h"

SolveProcess::SolveProcess()
{
    m_rootBoard = new BoardModel();
    m_moves = new MoveItems();
    m_hashes = new QVector<quint32>;
}

SolveProcess::~SolveProcess()
{
    clear();
    delete m_hashes;
    delete m_moves;
    delete m_rootBoard;
}

void SolveProcess::clear()
{
    m_moves->clear();
    m_hashes->clear();
    m_rootBoard->clear();
}

void SolveProcess::setRootBoard(BoardModel *startBoard)
{
    clear();
    if (startBoard) {
        m_rootBoard->cloneFrom(* startBoard);
        m_rootBoard->calculateHash();
        m_rootBoard->calculateMoves();
    }
}

void SolveProcess::stop()
{
    m_externalBreak = true;
}

void SolveProcess::doSolve(BoardModel *startBoard)
{
    if (startBoard)
        setRootBoard(startBoard);

    if (!m_rootBoard) {
        qDebug() << "Start board is undefined. Nothing to solve.";
        emit notSolved();
        return;
    }

    MoveItem * move = m_rootBoard->currentMove();
    if (!move) {
        qDebug() << "Start board has no any moves. Solve process cannot started.";
        emit notSolved();
        return;
    }

    qDebug() << "Start solving with" << m_rootBoard;

    m_moves->clear();
    m_hashes->clear();
    m_externalBreak = false;
    bool boardSolved = false;
    int  counter = 0;

    do {
        // A move is considered successful if after it:
        //  - the game is solved, or
        //  - there are more moves on the resulting board
        bool moveSuccess = move->doMove();
        counter ++;

        // Additional check of the resulting board,
        // if a similar game combination has occurred before
        if (moveSuccess)
            moveSuccess = !(m_hashes->contains( move->boardAfter()->hash() ));

        if (moveSuccess) {

            boardSolved = move->boardAfter()->isSolved();
            if (!boardSolved) {
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
                board->deleteCurrentMove();
                // and goes with the next ranked move
                move = board->currentMove();

                // if this board's moves are over
                if (!move)
                    // goes with the parent board
                    board = board->parentBoard();

            } while (!move && (board != nullptr));
        }

    } while (!m_externalBreak && !boardSolved && move);

    qDebug() << "== Solve process is finished ==\n"
             << "solved:" << boardSolved
             << "| external break:" << m_externalBreak
             << "| moves:" << counter
             << "| hashes:" << m_hashes->size();

    if (boardSolved) {

        if (move)
            qDebug() << move->boardAfter();

        while (move) {
            m_moves->prepend(new MoveItem(move->stored()));
            move = move->parent();
        }

        qDebug() << "Moves to solve:";
        for (int i = 0; i < m_moves->size(); ++i)
            qDebug().nospace() << "Move #" << i << " - " << m_moves->at(i);

        emit this->solved();

    } else {

        if (m_externalBreak) {
            qDebug() << "You have cancelled your search for a solution to the game.";
            emit this->interrupted();
        } else {
            qDebug() << "The game solution is not found. Try to undo some previous moves.";
            emit this->notSolved();
        }
    }

    m_hashes->clear();
    m_rootBoard->clear();
}
