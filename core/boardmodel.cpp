#include "boardmodel.h"
#include "src/ctglobal.h"
#include "src/game.h"
#include "core/usedcolors.h"
#include "core/jctlformat.h"

BoardModel::BoardModel()
{
    m_moves = new MoveItems;
    m_tubes = new TubeModels;
    m_parentBoard = nullptr;
    m_parentMove = nullptr;
    m_rootBoard = this;
}

BoardModel::BoardModel(BoardModel * parentBoard)
{
    BoardModel();
    if (parentBoard) {
        m_parentBoard = parentBoard;
        m_parentMove = parentBoard->currentMove();
        m_rootBoard = parentBoard->rootBoard();
    }
}

BoardModel::BoardModel(MoveItem * parentMove)
{
    BoardModel();
    if (parentMove) {
        m_parentMove = parentMove;
        m_parentBoard = parentMove->boardBefore();
        m_rootBoard = m_parentBoard->rootBoard();
    }
}

BoardModel::~BoardModel()
{
    clear();
    delete m_tubes;
    delete m_moves;
}

void BoardModel::clear()
{
    clearTubes();
    clearMoves();

    m_rootBoard = nullptr;
    m_parentBoard = nullptr;
    m_parentMove = nullptr;
}

void BoardModel::clearTubes()
{
    if (!m_tubes->empty())
        qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();
}

void BoardModel::clearMoves()
{
    if (!m_moves->empty())
        qDeleteAll(m_moves->begin(), m_moves->end());
    m_moves->clear();
}

MoveItem * BoardModel::currentMove()
{
    if (!m_moves->empty())
        return m_moves->last();
    return nullptr;
}

void BoardModel::deleteCurrentMove()
{
    if (!m_moves->empty()) {
        delete m_moves->last();
        m_moves->remove(m_moves->size() - 1);
    }
}

bool BoardModel::operator == (const BoardModel & other) const
{
    if (tubesCount() != other.tubesCount())
        return false;

    bool result = true;
    quint32 * store1 = new quint32[tubesCount()];
    quint32 * store2 = new quint32[tubesCount()];

    for (int i = 0; i < tubesCount(); ++i) {
        store1[i] = tubeAt(i)->store();
        store2[i] = other.tubeAt(i)->store();
    }

    int j1, j2;
    quint32 temp1, temp2;

    // sort store1 & store2
    for (int i = 1; i < tubesCount(); ++i) {
        temp1 = store1[i];
        temp2 = store2[i];
        j1 = j2 = i;
        while ( (j1 > 0) && (store1[j1-1] < temp1) ) {
            store1[j1] = store1[j1-1];
            j1--;
        }
        while ( (j2 > 0) && (store2[j2-1] < temp2) ) {
            store2[j2] = store2[j2-1];
            j2--;
        }
        store1[j1] = temp1;
        store2[j2] = temp2;
    }

    // compare sorted
    j1 = 0;
    while (result && j1 < tubesCount()) {
        if (store1[j1] != store2[j1])
            result = false;
        j1++;
    }

    delete [] store1;
    delete [] store2;

    return result;
}

TubeModel * BoardModel::tubeAt(int index) const
{
    if (index >= 0 && index < m_tubes->size())
        return m_tubes->at(index);
    return nullptr;
}

TubeModel * BoardModel::addNewTube()
{
    TubeModel * tube = new TubeModel();
    m_tubes->append(tube);
    return tube;
}

TubeModel * BoardModel::addNewTube(TubeModel * tubeToClone)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(tubeToClone);
    m_tubes->append(tube);
    return tube;
}

TubeModel * BoardModel::addNewTube(quint32 storedTube)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(storedTube);
    m_tubes->append(tube);
    return tube;
}

bool BoardModel::isSolved()
{
    bool result = true;
    quint8 index = 0;

    while (result && index < m_tubes->size()) {
        if (m_tubes->at(index)->state() == CT_STATE_FILLED
                || m_tubes->at(index)->state() == CT_STATE_REGULAR)
            result = false;
        index ++;
    }
    return result;
}

bool BoardModel::canDoMove(int tubeFromIndex, int tubeToIndex)
{
    if ( tubeFromIndex < 0 || tubeFromIndex >= m_tubes->size()
         || tubeToIndex < 0 || tubeToIndex >= m_tubes->size())
        return false;

    return canDoMove(m_tubes->at(tubeFromIndex), m_tubes->at(tubeToIndex));
}

bool BoardModel::canDoMove(TubeModel * tubeFrom, TubeModel * tubeTo)
{
    if (tubeFrom && tubeTo)
        return tubeTo->canPutColor(tubeFrom->currentColor());
    return false;
}

quint8 BoardModel::colorsToMove(int tubeFromIndex, int tubeToIndex)
{
    if ( tubeFromIndex < 0 || tubeFromIndex >= m_tubes->size()
         || tubeToIndex < 0 || tubeToIndex >= m_tubes->size())
        return false;
    return colorsToMove(m_tubes->at(tubeFromIndex), m_tubes->at(tubeToIndex));
}

quint8 BoardModel::colorsToMove(TubeModel * tubeFrom, TubeModel * tubeTo)
{
    if (canDoMove(tubeFrom, tubeTo))
        return qMin(tubeFrom->currentColorCount(),
                    tubeTo->rest());
    else
        return 0;
}

quint32 BoardModel::getMoveData(int tubeFromIndex, int tubeToIndex)
{
    if (!canDoMove(tubeFromIndex, tubeToIndex))
        return 0;

    MoveItem::MoveData move;
    move.fields.tubeFrom = tubeFromIndex;
    move.fields.tubeTo = tubeToIndex;
    move.fields.count = colorsToMove(tubeFromIndex, tubeToIndex);
    move.fields.color = m_tubes->at(tubeFromIndex)->currentColor();
    return move.stored;
}

MoveItem * BoardModel::addNewMove(int tubeFromIndex, int tubeToIndex)
{
    MoveItem * move = new MoveItem(this, tubeFromIndex, tubeToIndex);
    m_moves->append(move);
    return move;
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
    int i = 0;
    while (i < m_tubes->size()) {
        if (m_tubes->at(i)->isDone())
            return false;
        i++;
    }
    CtGlobal::game().jctl()->storeGame(this);
    return CtGlobal::game().jctl()->checkTubes();
}

void BoardModel::fillActiveColors()
{
    CtGlobal::game().usedColors()->clearAllUsed();
    for (int i = 0; i < tubesCount(); ++i) {
//        if (!m_tubes->at(i)->isDone())
            CtGlobal::game().usedColors()->incUsed(
                        m_tubes->at(i)->currentColor(),
                        m_tubes->at(i)->currentColorCount());
    }
}

quint16 BoardModel::calculateMoves()
{
    TubeModel * tubeFrom;
    TubeModel * tubeTo;
    MoveItem  * move;

    quint8 result = 0;                   // number of available moves
    bool emptyTubeProcessed = false;     // true if one of empty tube has processed already

    clearMoves();
    fillActiveColors();

    for (quint8 tubeToIndex = 0; tubeToIndex < tubesCount(); ++tubeToIndex) {

        tubeTo = m_tubes->at(tubeToIndex);
        if ( (tubeTo->state() == CT_STATE_EMPTY && !emptyTubeProcessed)
                || tubeTo->state() == CT_STATE_REGULAR) {

            if (!emptyTubeProcessed && tubeTo->isEmpty())
                emptyTubeProcessed = true;

            quint8 toCount = tubeTo->currentColorCount();

            for (quint8 tubeFromIndex = 0; tubeFromIndex < tubesCount(); ++tubeFromIndex) {

                if (tubeFromIndex != tubeToIndex
                        && canDoMove(tubeFromIndex, tubeToIndex)) {

                    tubeFrom = m_tubes->at(tubeFromIndex);
                    if (tubeTo->canPutColor(tubeFrom->currentColor())) {

                        quint8 fromCount = tubeFrom->currentColorCount();

                        move = addNewMove(tubeFromIndex, tubeToIndex);
                        qint8 rank = move->count() + 100 - result;





                        move->setRank(rank);
                        result ++;
                    }
                }
            }
        }
    }

    if (result > 1)
        m_moves->sortByRank();

    // --- debug
    qDebug() << this; // out current board
    qDebug() << "found" << result << "moves.";
    for (int i=0; i < m_moves->size(); ++i) {
        qDebug() <<"#" << i << (m_moves->at(i));
    }

    return result;
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
