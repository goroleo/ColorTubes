#include "moveitem.h"
#include "boardmodel.h"
#include "tubemodel.h"

MoveItem::MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo)
{
    rank = 0;
    if (board) {
        m_boardBefore = board;
        m_parentMove = board->parentMove();
        m_data.stored = board->getMoveData(idxTubeFrom, idxTubeTo);
    }
}

MoveItem::MoveItem(quint32 storedMove)
{
    rank = 0;
    m_data.stored = storedMove;
}

MoveItem::~MoveItem()
{
    if (m_boardAfter)
        delete m_boardAfter;
    if (m_children) {
        delete m_children;
    }
}

bool MoveItem::doMove()
{
    if (m_data.stored == 0)
        return false;

    m_boardAfter = new BoardModel(this);

    bool result = m_boardAfter->isSolved() || m_boardAfter->calculateMoves() > 0;
    if (!result) {
        delete m_boardAfter;
        m_boardAfter = nullptr;
    }
    return result;
}

bool MoveItem::hasChildren()
{
    if (m_children)
        return !(m_children->isEmpty());
    return false;
}

int MoveItem::childrenCount()
{
    if (m_children)
        return m_children->size();
    return 0;
}

MoveItem * MoveItem::currentChild()
{
    if (m_children)
        return m_children->current();
    return nullptr;
}

void MoveItem::addChild(quint32 storedMove)
{
    addChild(new MoveItem(storedMove));
}

void MoveItem::addChild(MoveItem * move)
{
    if (!m_children)
        m_children = new MoveItems();
    move->m_parentMove = this;
    move->m_boardBefore = this->m_boardAfter;
    m_children->append(move);
}

void MoveItem::removeLastChild()
{
    if (m_children)
        m_children->removeLast();
}

MoveItem * MoveItems::current()
{
    if (!isEmpty())
        return last();
    return nullptr;
}


MoveItems::~MoveItems()
{
    clear();
}


void MoveItems::removeLast()
{
   if (!isEmpty()) {
       delete last();
       remove (size() - 1);
   }
}

void MoveItems::clear()
{
    while (!isEmpty())
        removeLast();
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
        while ( (j > 0) && (at(j-1)->rank > temp->rank) ) {
            move(j-1, j);
            j--;
        }
        replace(j, temp);
    }
}

QDebug operator << (QDebug dbg, const MoveItem & moveItem)
{
    dbg.nospace() << "MoveItem { "
        << "from: "    << QString::number(moveItem.tubeFrom(), 16)
        << ", to: "    << QString::number(moveItem.tubeTo(), 16)
        << ", color: " << QString::number(moveItem.color(), 16)
        << ", count: " << moveItem.count()
        << ", rank: "  << moveItem.rank
        << " }";
    return dbg.maybeSpace();
}

QDebug operator << (QDebug dbg, const MoveItem * moveItem)
{
    dbg.nospace() << * moveItem;
    return dbg.maybeSpace();
}
