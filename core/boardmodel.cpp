#include "boardmodel.h"
#include "src/ctglobal.h"
#include "src/game.h"
#include "core/usedcolors.h"
#include "core/jctlformat.h"

BoardModel::BoardModel()
{
    m_tubes = new TubeModels;
    m_parentBoard = nullptr;
    m_parentMove = nullptr;
    m_rootBoard = this;
    m_moves = nullptr;
    m_hash = 0;
}

BoardModel::BoardModel(MoveItem * move)
{
    BoardModel();
    if (move) {
        m_parentMove = move;
        m_parentBoard = move->boardBefore();
        m_rootBoard = m_parentBoard->rootBoard();
        for (quint8 i = 0; i < m_parentBoard->tubesCount(); ++i )
            addNewTube(m_parentBoard->m_tubes->at(i));
        for (int i = 0; i < move->count(); ++i) {
            m_tubes->at(move->tubeFrom())->extractColor();
            m_tubes->at(move->tubeTo())->putColor(move->color());
        }
        calculateHash();
    }
}

BoardModel::~BoardModel()
{
    clear();
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
    m_hash = 0;
}

MoveItems * BoardModel::moves()
{
    if (m_parentMove)
        return m_parentMove->children();
    return m_moves;
}

int BoardModel::movesCount()
{
    if (m_parentMove)
        return m_parentMove->childrenCount();
    if (m_moves)
        return m_moves->size();
    return 0;
}

bool BoardModel::hasMoves()
{
    if (m_parentMove)
        return m_parentMove->hasChildren();
    if (m_moves)
        return !m_moves->isEmpty();
    return 0;
}

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

/*
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
*/

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
    quint8 index = 0;

    while (index < m_tubes->size()) {
        if (m_tubes->at(index)->state() == CT_STATE_FILLED
                || m_tubes->at(index)->state() == CT_STATE_REGULAR)
            return false;
        index ++;
    }
    return true;
}

bool BoardModel::canDoMove(int tubeFromIndex, int tubeToIndex)
{
    if ( tubeFromIndex < 0 || tubeFromIndex >= m_tubes->size()
         || tubeToIndex < 0 || tubeToIndex >= m_tubes->size()
         || tubeToIndex == tubeFromIndex)
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
        return 0;
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
    MoveData move;
    move.fields.count = colorsToMove(tubeFromIndex, tubeToIndex);
    if (move.fields.count > 0) {
        move.fields.tubeFrom = tubeFromIndex;
        move.fields.tubeTo = tubeToIndex;
        move.fields.color = m_tubes->at(tubeFromIndex)->currentColor();
        return move.stored;
    }
    return 0;
}

MoveItem * BoardModel::addNewMove(int tubeFromIndex, int tubeToIndex)
{
    MoveItem * move = new MoveItem(this, tubeFromIndex, tubeToIndex);
    if (m_parentMove)
        m_parentMove->addChild(move);
    else {
        if (!m_moves)
            m_moves = new MoveItems;
        m_moves->append(move);
    }
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
//      if (!m_tubes->at(i)->isDone())
        CtGlobal::game().usedColors()
                ->incUsed(m_tubes->at(i)->currentColor(),
                          m_tubes->at(i)->currentColorCount());
    }
}

void BoardModel::calculateHash()
{

    ColorCells * cells = new ColorCells[m_tubes->size()];

    for (int i = 0; i < m_tubes->size(); ++i)
        cells[i].stored = m_tubes->at(i)->store();

    int j;
    quint32 temp;

    // sort
    for (int i = 1; i < m_tubes->size(); ++i) {
        temp = cells[i].stored;
        j = i;
        while ( (j > 0) && (cells[j-1].stored < temp) ) {
            cells[j].stored = cells[j-1].stored;
            j--;
        }
        cells[j].stored = temp;
    }

    m_hash = 0xffff;
    bool doXOR;

    for (int i = 0; i < tubesCount(); ++i)
        for (int j = 3; j >= 0; j--) {
            m_hash ^= cells[i].items[j];
            for (int b = 0; b < 8; b++) {
                doXOR = ((m_hash & 1) != 0);
                m_hash >>= 1;
                if (doXOR)
                    m_hash ^= 0xa001;
            }
        }

    delete [] cells;
}

quint16 BoardModel::calculateMoves()
{
    TubeModel * tubeFrom;
    TubeModel * tubeTo;
    MoveItem  * move;

    quint8 result = 0;                   // number of available moves
    bool emptyTubeProcessed = false;     // true if one of empty tube has processed already

    if (moves())
        moves()->clear();

    fillActiveColors();

    for (quint8 tubeToIndex = 0; tubeToIndex < tubesCount(); ++tubeToIndex) {

        tubeTo = m_tubes->at(tubeToIndex);
        if ( (tubeTo->state() == CT_STATE_EMPTY && !emptyTubeProcessed)
                || tubeTo->state() == CT_STATE_REGULAR) {

            if (tubeTo->isEmpty())
                emptyTubeProcessed = true;

            quint8 toCount = tubeTo->currentColorCount();

            for (quint8 tubeFromIndex = 0; tubeFromIndex < tubesCount(); ++tubeFromIndex) {

                if (tubeFromIndex != tubeToIndex
                        && canDoMove(tubeFromIndex, tubeToIndex)) {

                    tubeFrom = m_tubes->at(tubeFromIndex);
                    quint8 fromCount = tubeFrom->currentColorCount();

                    move = addNewMove(tubeFromIndex, tubeToIndex);

                    if (fromCount + toCount == tubeFrom->count() + tubeTo->count())
                        move->rank = tubeFrom->count() + tubeTo->count() - move->count();
                    else
                        move->rank = qMin(tubeTo->rest(),
                                    CtGlobal::game().usedColors()->getUsed(move->color()));

                    if (toCount > 0 && toCount == tubeTo->count())
                        move->rank += 4;

                    if (fromCount == tubeFrom->count())
                        move->rank += 2;

                    if (fromCount > tubeTo->rest())
                        move->rank -= 4;

                    if (fromCount == tubeFrom->count() && tubeTo->isEmpty())
                        move->rank -= 5;

                    result ++;
                }
            }
        }
    }

    if (result > 1)
        moves()->sortByRank();

    // --- debug
    qDebug() << this; // out current board
    calculateHash();
    qDebug().noquote() << "Hash" << QString::number(m_hash, 16);

    qDebug() << "Found" << result << "possible moves.";
    for (int i=0; i < moves()->size(); ++i) {
        qDebug() <<"#" << i << (moves()->at(i));
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
