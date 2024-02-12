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
    m_tubeFrom = idxTubeFrom;
    m_tubeTo = idxTubeTo;
    m_parentMove = board->parentMove;
    m_color = m_boardBefore->tubes->at(idxTubeFrom).currentColor();
    m_count = qMin( board->tubes->at(idxTubeFrom).sameColorsCount(),
                   quint8(4 - board->tubes->at(idxTubeTo).count()) );
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
    m_tubeFrom = idxTubeFrom;
}

void MoveItem::setTubeTo(quint8 idxTubeTo)
{
    m_tubeFrom = idxTubeTo;
}

void MoveItem::setRank(qint8 newRank)
{
    m_rank = newRank;
}

quint32 MoveItem::storeMove() {
    return ((m_tubeFrom & 0xff) << 24)
           | ((m_tubeTo & 0xff) << 16)
           | ((m_count & 0xff) << 8)
           | ((m_color) & 0xff);
}

