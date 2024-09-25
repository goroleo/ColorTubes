#include "boardmodel.h"
#include "src/ctglobal.h"
#include "src/game.h"
#include "core/usedcolors.h"
#include "core/jctlformat.h"

BoardModel::BoardModel(MoveItem * fromMove)
{
    m_tubes = new TubeModels();

    if (fromMove) {

        // links to previous board
        m_parentMove = fromMove;
        m_parentBoard = fromMove->boardBefore();
        m_rootBoard = m_parentBoard->rootBoard();

        // fills tubes
        this->cloneFrom(* m_parentBoard);

        // does move
        for (int i = 0; i < fromMove->count(); ++i) {
            m_tubes->at(fromMove->tubeFrom())->extractColor();
            m_tubes->at(fromMove->tubeTo())->putColor(fromMove->color());
        }

        // calculates hash value
        calculateHash();
    }
}

BoardModel::~BoardModel()
{
    delete m_tubes;
    if (m_moves)
        delete m_moves;
}

void BoardModel::clear()
{
    m_tubes->clear();
    if (m_moves)
        m_moves->clear();

    m_rootBoard = nullptr;
    m_parentBoard = nullptr;
    m_parentMove = nullptr;
    m_crc32 = 0;
}

void BoardModel::cloneFrom(const BoardModel &boardToClone)
{
    for (int i = 0; i < boardToClone.m_tubes->size(); ++i)
        addNewTube( *(boardToClone.m_tubes->at(i)));
}

bool BoardModel::isSolved()
{
    for (TubeModel *tube : *m_tubes) {
        if (tube->state() == CT_STATE_FILLED
                || tube->state() == CT_STATE_REGULAR)
            return false;
    }
    return true;
}

TubeModel * BoardModel::tubeAt(int index) const
{
    if (index >= 0 && index < m_tubes->size())
        return m_tubes->at(index);
    return nullptr;
}

void BoardModel::addNewTube()
{
    TubeModel * tube = new TubeModel();
    m_tubes->append(tube);
}

void BoardModel::addNewTube(const TubeModel &tubeToClone)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(tubeToClone);
    m_tubes->append(tube);
}

void BoardModel::addNewTube(quint32 storedTube)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(storedTube);
    m_tubes->append(tube);
}

MoveItems * BoardModel::moves()
{
    if (m_parentMove)
        return m_parentMove->children();
    return m_moves;
}

MoveItem * BoardModel::addNewMove(const TubeModel &tubeFrom, const TubeModel &tubeTo)
{
    MoveItem * move = new MoveItem(this, getMoveData(tubeFrom, tubeTo));
    if (m_parentMove)
        m_parentMove->addChild(move);
    else {
        if (!m_moves)
            m_moves = new MoveItems;
        m_moves->append(move);
    }
    return move;
}

/* unused
int BoardModel::movesCount()
{
    if (m_parentMove)
        return m_parentMove->childrenCount();
    if (m_moves)
        return m_moves->size();
    return 0;
}
*/

/* unused
bool BoardModel::hasMoves()
{
    if (m_parentMove)
        return m_parentMove->hasChildren();
    if (m_moves)
        return !m_moves->isEmpty();
    return false;
}
*/

MoveItem * BoardModel::currentMove()
{
    if (m_parentMove)
        return m_parentMove->currentChild();
    if (m_moves)
        return m_moves->current();
    return nullptr;
}

void BoardModel::deleteCurrentMove()
{
    if (m_parentMove)
        m_parentMove->removeLastChild();
    if (m_moves)
        m_moves->removeLast();
}

bool BoardModel::canDoMove(const TubeModel &tubeFrom, const TubeModel &tubeTo)
{
    if (&tubeFrom != &tubeTo)
        return tubeTo.canPutColor(tubeFrom.currentColor());
    return false;
}

quint8 BoardModel::colorsToMove(const TubeModel &tubeFrom, const TubeModel &tubeTo)
{
    if (canDoMove(tubeFrom, tubeTo))
        return qMin(tubeFrom.currentColorCount(),
                    tubeTo.rest());
    else
        return 0;
}

quint32 BoardModel::getMoveData(const TubeModel &tubeFrom, const TubeModel &tubeTo)
{
    MoveData move;
    move.fields.count = colorsToMove(tubeFrom, tubeTo);
    if (move.fields.count > 0) {
        move.fields.tubeFrom = m_tubes->indexOf( (TubeModel *) &tubeFrom);
        move.fields.tubeTo = m_tubes->indexOf( (TubeModel *) &tubeTo);
        move.fields.color = tubeFrom.currentColor();
        return move.stored;
    }
    return 0;
}

quint16 BoardModel::calculateMoves()
{

    quint16 result = 0;                  // number of available moves
    bool emptyTubeProcessed = false;     // true if one of empty tube has processed already

    if (moves())
        moves()->clear();

    fillActiveColors();

    for (const TubeModel * tubeTo : * m_tubes) {

        if ( (tubeTo->state() == CT_STATE_EMPTY && !emptyTubeProcessed)
             || tubeTo->state() == CT_STATE_REGULAR) {

            quint8 toCount = tubeTo->currentColorCount();

            for (const TubeModel * tubeFrom : * m_tubes) {

                if (canDoMove(*tubeFrom, *tubeTo)) {

                    quint8 fromCount = tubeFrom->currentColorCount();

                    // create & add new available move
                    MoveItem * move = addNewMove(*tubeFrom, *tubeTo);

                    // --> rank the move!
                    if (fromCount + toCount == tubeFrom->count() + tubeTo->count())
                        // if both tubes are filled with only one color
                        move->rank = tubeFrom->count() + tubeTo->count() - move->count();
                    else
                        move->rank = qMin(tubeTo->rest(),
                                          CtGlobal::game().usedColors()->getUsed(move->color()));

                    if (toCount > 0 && toCount == tubeTo->count())
                        // if recipient tube is filled with only one color
                        move->rank += 4;

                    if (fromCount == tubeFrom->count())
                        // if donator tube is filled with only one color
                        move->rank += 2;

                    if (fromCount > tubeTo->rest())
                        // if after this move there are cells filled with move's color left in the donor tube
                        move->rank -= 4;

                    if (fromCount == tubeFrom->count() && tubeTo->isEmpty())
                        //
                        move->rank -= 5;
                    // <-- end move ranking

                    result ++;
                }
            }

            if (tubeTo->isEmpty())
                emptyTubeProcessed = true;
        }
    }

    if (result > 1)
        moves()->sortByRank();

/*
    // --- debug
    qDebug() << this; // out current board
    qDebug().noquote() << "Hash" << QString::number(m_hash, 16);

    qDebug() << "Found" << result << "possible moves.";
    if (hasMoves()) {

        for (int i=0; i < moves()->size(); ++i) {
            qDebug() <<"#" << i << (moves()->at(i));
        }
    }
*/
    return result;
}

void BoardModel::randomFill(int fillTubes, int emptyTubes)
{
    do {
        clear();
        CtGlobal::game().usedColors()->clearAllUsed();

        m_rootBoard = this;
        for (int i = 0; i < fillTubes + emptyTubes; ++i)
            addNewTube();

        for (int i = 0; i < fillTubes; ++i) {
            for (int c = m_tubes->at(i)->count(); c < 4; ++c) {
                quint8 clr = CtGlobal::game().usedColors()->getRandomColor();
                m_tubes->at(i)->putColor(clr);
                CtGlobal::game().usedColors()->incUsed(clr);
                if (CtGlobal::game().usedColors()->numberOfUsedColors() >= fillTubes) {
                    CtGlobal::game().usedColors()->disableUnusedColors();
                }
            }
        }
    } while (!checkFilledTubes());
}

bool BoardModel::checkFilledTubes()
{
    if (m_tubes->isEmpty())
        return false;

    for (const TubeModel *tube : *m_tubes) {
        if (tube->isDone())
            return false;
    }

    CtGlobal::game().jctl()->storeGame(this);
    return CtGlobal::game().jctl()->checkTubes();
}

void BoardModel::fillActiveColors()
{
    CtGlobal::game().usedColors()->clearAllUsed();
    for (int i = 0; i < m_tubes->size(); ++i) {
        CtGlobal::game().usedColors()
                ->incUsed(m_tubes->at(i)->currentColor(),
                          m_tubes->at(i)->currentColorCount());
    }
}

void BoardModel::calculateHash()
{
    ColorCells cells [m_tubes->size()];
    for (int i = 0; i < m_tubes->size(); ++i)
        cells[i].stored = m_tubes->at(i)->store();

    // sort
    int j;
    quint32 temp;
    for (int i = 1; i < m_tubes->size(); ++i) {
        temp = cells[i].stored;
        j = i;
        while ( (j > 0) && (cells[j-1].stored < temp) ) {
            cells[j].stored = cells[j-1].stored;
            j--;
        }
        cells[j].stored = temp;
    }

    // CRC32 hash value
    m_crc32 = 0xffffffff;
    for (int i = 0; i < m_tubes->size(); ++i) {
        for (int j = 3; j >= 0; j--) {
            quint8 byte = cells[i].items[j];
            m_crc32 = CtGlobal::crc32Table( (m_crc32 ^ byte) & 0xFF) ^ (m_crc32 >> 8);
        }
    }
    m_crc32 ^= 0xffffffff;
}

QString BoardModel::toString() const
{
    if (tubesCount() == 0)
        return "";

    QString str("BoardModel:\n");

    str.append("  ");
    for (int i = 0; i < tubesCount(); i++) {
        str.append(QString::number(i, 16)).append("   ");
    }
    str.append("\n");

    for (int i = 3; i >= 0; i--) {
        str.append("| ");

        for (quint8 j = 0; j < tubesCount(); ++j) {
            if (tubeAt(j)->color(i) != 0) {
                str.append(QString::number(tubeAt(j)->color(i), 16));
            } else {
                str.append(" ");
            }
            str.append(" | ");
        }
        if (i != 0) str.append("\n");
    }
    return str;
}

QDebug operator << (QDebug dbg, const BoardModel &board)
{
    dbg.noquote().nospace() << board.toString();
    return dbg.maybeSpace();
}

QDebug operator << (QDebug dbg, const BoardModel *board)
{
    dbg.noquote().nospace() << board->toString();
    return dbg.maybeSpace();
}
