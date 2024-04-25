#include "moveitem.h"
#include "boardmodel.h"
#include "tubemodel.h"

#include <math.h>

MoveItem::MoveItem()
{
}

MoveItem::MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo)
{
    m_boardBefore = board;
    m_parentMove = board->parentMove();
    m_data.stored = board->getMove(idxTubeFrom, idxTubeTo);
    m_rank = 0;
}

MoveItem::~MoveItem()
{
    if (m_boardAfter)
        delete m_boardAfter;
}

bool MoveItem::doMove()
{
    return true;
}

bool MoveItem::undoMove()
{
    return true;
}

void MoveItem::setParentMove(MoveItem *item)
{
    m_parentMove = item;
}

void MoveItem::setBoardBefore(BoardModel *bm)
{
    m_boardBefore = bm;
}

void MoveItem::setTubeFrom(quint8 idxTubeFrom)
{
    m_data.fields.tubeFrom = idxTubeFrom;
}

void MoveItem::setTubeTo(quint8 idxTubeTo)
{
    m_data.fields.tubeTo = idxTubeTo;
}

void MoveItem::setRank(qint8 newRank)
{
    m_rank = newRank;
}

quint32 MoveItem::storeMove()
{
    return m_data.stored;
}
