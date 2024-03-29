#include "boardmodel.h"

BoardModel::BoardModel()
{
    m_moves = new QVector<MoveItem *>;
    m_tubes = new QVector<TubeModel *>;
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

bool BoardModel::hasMoves() const
{
    return !m_moves->empty();
}

MoveItem * BoardModel::currentMove() const
{
    if (!m_moves->empty())
        return (m_moves->last());
    return nullptr;
}

void BoardModel::deleteCurrentMove()
{
    if (!m_moves->empty())
        m_moves->remove(m_moves->size() - 1);
}

bool BoardModel::operator == (const BoardModel & other) const
{
    if (tubesCount() != other.tubesCount())
        return false;

    bool result = true;
    quint32 * stored1 = new quint32[tubesCount()];
    quint32 * stored2 = new quint32[tubesCount()];

    for (int i = 0; i < tubesCount(); ++i) {
        stored1[i] = getTube(i)->store();
        stored2[i] = other.getTube(i)->store();
    }

    int j;
    quint32 temp;

//    std::sort(stored1[0], stored1[tubesCount()-1]);
//    std::sort(stored2[0], stored2[tubesCount()-1]);

    // sort stored1
    for (int i = 1; i < tubesCount(); ++i) {
        temp = stored1[i];
        j = i;
        while ( (j > 0) && (stored1[j-1] < temp) ) {
            stored1[j] = stored1[j-1];
            j--;
        }
        stored1[j] = temp;
    }

    // sort stored2
    for (int i = 1; i < tubesCount(); ++i) {
        temp = stored2[i];
        j = i;
        while ( (j > 0) && (stored2[j-1] < temp) ) {
            stored2[j] = stored2[j-1];
            j--;
        }
        stored2[j] = temp;
    }

    for (int i = 0; i < tubesCount(); ++i) {
        if (stored1[i] != stored2[i])
            result = false;
    }

    delete [] stored1;
    delete [] stored2;

    return result;
}

TubeModel * BoardModel::getTube(int index) const
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

    QString str("\n  ");
    for (int i = 0; i < m_tubes->size(); i++) {
        str.append(QString::number(i, 16)).append("   ");
    }
    str.append("\n");

    for (int i = 3; i >= 0; i--) {
        str.append("| ");

        for (quint8 j = 0; j < m_tubes->size(); ++j) {

            if (getTube(j)->color(i) != 0) {
                str.append(QString::number(getTube(j)->color(i), 16));
            } else {
                str.append(" ");
            }
            str.append(" | ");
        }
        str.append("\n");
    }
    return str;
}

bool BoardModel::isSolved()
{
    bool result = true;
    quint8 index = 0;

    while (result && index < m_tubes->size()) {
        if (m_tubes->at(index)->state() == TubeModel::STATE_FILLED
                || m_tubes->at(index)->state() == TubeModel::STATE_REGULAR)
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

    return m_tubes->at(tubeToIndex)->canPutColor(m_tubes->at(tubeToIndex)->currentColor());
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
    if (canDoMove(tubeFromIndex, tubeToIndex))
        return ((tubeFromIndex & 0xff) << 24)                               // tube from
                | ((tubeToIndex & 0xff) << 16)                              // tube to
                | ((colorsToMove(tubeFromIndex, tubeToIndex) & 0xff) << 8)  // count
                | ((m_tubes->at(tubeFromIndex)->currentColor()) & 0xff);    // color
    else
        return 0;
}
