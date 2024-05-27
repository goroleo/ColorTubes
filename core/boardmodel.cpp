#include "boardmodel.h"
#include "src/ctglobal.h"

BoardModel::BoardModel()
{
    m_moves = new GameMoves;
    m_tubes = new GameTubes;
}

BoardModel::BoardModel(BoardModel * parentBoard)
{
    BoardModel();
    m_parentBoard = parentBoard;
    m_parentMove = parentBoard->currentMove();
    m_rootBoard = parentBoard->rootBoard();
}

BoardModel::BoardModel(MoveItem * parentMove)
{
    BoardModel();
    m_parentMove = parentMove;
    m_parentBoard = parentMove->boardBefore();
    m_rootBoard = m_parentBoard->rootBoard();
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
    qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();
}

void BoardModel::clearMoves()
{
    qDeleteAll(m_moves->begin(), m_moves->end());
    m_moves->clear();
}

MoveItem * BoardModel::currentMove()
{
    if (!m_moves->empty())
        return (m_moves->last());
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
    quint32 * stored1 = new quint32[tubesCount()];
    quint32 * stored2 = new quint32[tubesCount()];

    for (int i = 0; i < tubesCount(); ++i) {
        stored1[i] = tubeAt(i)->store();
        stored2[i] = other.tubeAt(i)->store();
    }

    int j1, j2;
    quint32 temp1, temp2;

    // sort stored1 & 2
    for (int i = 1; i < tubesCount(); ++i) {
        temp1 = stored1[i];
        temp2 = stored2[i];
        j1 = j2 = i;
        while ( (j1 > 0) && (stored1[j1-1] < temp1) ) {
            stored1[j1] = stored1[j1-1];
            j1--;
        }
        while ( (j2 > 0) && (stored2[j2-1] < temp2) ) {
            stored2[j2] = stored2[j2-1];
            j2--;
        }
        stored1[j1] = temp1;
        stored2[j2] = temp2;
    }

    for (int i = 0; i < tubesCount(); ++i) {
        if (stored1[i] != stored2[i])
            result = false;
    }

    delete [] stored1;
    delete [] stored2;

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

TubeModel * BoardModel::addNewTube(TubeModel * cloneTube)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(cloneTube);
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

QString BoardModel::toString() {

    if (m_tubes->isEmpty())
        return QString("");

    QString str("");

    str.append(CtGlobal::endOfLine()).append("  ");
    for (int i = 0; i < m_tubes->size(); i++) {
        str.append(QString::number(i, 16)).append("   ");
    }
    str.append(CtGlobal::endOfLine());

    for (int i = 3; i >= 0; i--) {
        str.append("| ");

        for (quint8 j = 0; j < m_tubes->size(); ++j) {
            if (tubeAt(j)->color(i) != 0) {
                str.append(QString::number(tubeAt(j)->color(i), 16));
            } else {
                str.append(" ");
            }
            str.append(" | ");
        }
        str.append(CtGlobal::endOfLine());
    }
    return str;
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

    return m_tubes->at(tubeToIndex)->canPutColor(m_tubes->at(tubeFromIndex)->currentColor());
}

quint8 BoardModel::colorsToMove(int tubeFromIndex, int tubeToIndex)
{
    if (canDoMove(tubeFromIndex, tubeToIndex))
        return qMin(m_tubes->at(tubeFromIndex)->sameColorCount(),
                    m_tubes->at(tubeToIndex)->rest());
    else
        return 0;
}

quint32 BoardModel::getMove(int tubeFromIndex, int tubeToIndex)
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

quint32 BoardModel::addNewMove(int tubeFromIndex, int tubeToIndex)
{
    MoveItem * move = new MoveItem(this, tubeFromIndex, tubeToIndex);
    m_moves->append(move);
    return move->stored();
}

quint32 BoardModel::undoMove()
{
    MoveItem * current = currentMove();
    if (current) {
        quint32 result = current->stored();
        for (int i=0; i < current->count(); ++i) {
            m_tubes->at(current->tubeTo())->extractColor();
            m_tubes->at(current->tubeFrom())->putColor(current->color());
        }
        deleteCurrentMove();
        return result;
    }
    return 0;
}


void BoardModel::startAgain()
{
    MoveItem * current;
    while (currentMove()) {
        current = currentMove();
        for (int i=0; i < current->count(); ++i) {
            m_tubes->at(current->tubeTo())->extractColor();
            m_tubes->at(current->tubeFrom())->putColor(current->color());
        }
        deleteCurrentMove();
    }
}
