#include "moveitem.h"
#include "boardmodel.h"
#include "tubemodel.h"

MoveItem::MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo)
{
    m_boardBefore = board;
    m_parentMove = board->parentMove();
    m_data.stored = board->getMove(idxTubeFrom, idxTubeTo);
    m_rank = 0;
    qDebug() << "New move added. From" << idxTubeFrom << "to" << idxTubeTo;
}

MoveItem::~MoveItem()
{
    if (m_boardAfter)
        delete m_boardAfter;
    qDebug() << "Move deleted. From" << m_data.fields.tubeFrom << "to" << m_data.fields.tubeTo;
}

bool MoveItem::doMove()
{
    return true;
}

quint32 MoveItem::stored()
{
    return m_data.stored;
}
