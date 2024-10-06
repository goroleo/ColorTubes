#include "moveitem.h"
#include "boardmodel.h"
#include "tubemodel.h"

MoveItem::MoveItem(BoardModel * board, const TubeModel &tubeFrom, const TubeModel &tubeTo)
{
    if (board) {
        m_boardBefore = board;
        m_parentMove = board->parentMove();
        m_data.stored = board->getMoveData(tubeFrom, tubeTo);
    }
}

MoveItem::MoveItem(BoardModel * board, quint32 storedMove)
{
    if (board) {
        m_boardBefore = board;
        m_parentMove = board->parentMove();
    }
    m_data.stored = storedMove;
}

MoveItem::MoveItem(quint32 storedMove)
{
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

    // A move is considered successful if after it:
    //  - the game is solved, or
    //  - there are more moves on the resulting board
    bool success = m_boardAfter->isSolved()
            || m_boardAfter->findMoves() > 0;

    if (!success) {
        delete m_boardAfter;
        m_boardAfter = nullptr;
    }

    return success;
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

void MoveItem::removeChildren()
{
    if (m_children) {
        delete m_children;
        m_children = nullptr;
    }
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

void MoveItems::removeFirst()
{
   if (!isEmpty()) {
       delete first();
       remove (0);
   }
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

QDebug operator << (QDebug dbg, const MoveItem &moveItem)
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

QDebug operator << (QDebug dbg, const MoveItem *moveItem)
{
    dbg.nospace() << *moveItem;
    return dbg.maybeSpace();
}
