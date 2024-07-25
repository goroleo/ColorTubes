#include "moveitem.h"
#include "boardmodel.h"
#include "tubemodel.h"

MoveItem::MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo)
{
    m_boardBefore = board;
    m_parentMove = board->parentMove();
    m_data.stored = board->getMove(idxTubeFrom, idxTubeTo);
    m_rank = 0;
//    qDebug() << "New move added." << this;
}

MoveItem::~MoveItem()
{
    if (m_boardAfter)
        delete m_boardAfter;
//    qDebug() << "Move deleted." << this;
}

bool MoveItem::doMove()
{
    m_boardAfter = new BoardModel(m_boardBefore);
    for (int i = 0; i < m_data.fields.count; ++i) {
        m_boardAfter->tubeAt(m_data.fields.tubeFrom)->extractColor();
        m_boardAfter->tubeAt(m_data.fields.tubeTo)->putColor(m_data.fields.color);
    }

    bool result = m_boardAfter->isSolved() || m_boardAfter->calculateMoves() > 0;
    if (!result)
        delete m_boardAfter;
    return result;
}

void MoveItems::sortByRank()
{
    if (size() < 2)
        return;

    quint8 j;
    MoveItem * temp;
    for (quint8 i = 1; i < size(); ++i) {
        temp = at(i);
        j = i;
        while ( (j > 0) && (at(j-1)->rank() > temp->rank()) ) {
            move(j-1, j);
            j--;
        }
        replace(j, temp);
    }
}

MoveItem * MoveItems::current()
{
    if (!empty())
        return last();
    return nullptr;
}

void MoveItems::clear()
{
    if (!empty())
        qDeleteAll(begin(), end());
    QVector::clear();
}

QDebug operator << (QDebug dbg, const MoveItem & moveItem)
{
    dbg.nospace() << "MoveItem { "
        << "from: "    << QString::number(moveItem.tubeFrom(), 16)
        << ", to: "    << QString::number(moveItem.tubeTo(), 16)
        << ", color: " << QString::number(moveItem.color(), 16)
        << ", count: " << moveItem.count()
        << ", rank: "  << moveItem.rank()
        << " }";
    return dbg.maybeSpace();
}

QDebug operator << (QDebug dbg, const MoveItem * moveItem)
{
    dbg.nospace() << * moveItem;
    return dbg.maybeSpace();
}
