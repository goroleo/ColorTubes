#include "gameboard.h"

#include "src/ctglobal.h"
#include "src/game.h"
#include "core/boardmodel.h"

GameBoard::GameBoard(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_model = CtGlobal::game().boardModel();
}

GameBoard::~GameBoard()
{

}

void GameBoard::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    qDebug() << "oldWidth" << oldGeometry.width() << "| oldHeight" << oldGeometry.height();
    qDebug() << "newWidth" << newGeometry.width() << "| newHeight" << newGeometry.height();
    qDebug() << "curWidth" << width() << "| curHeight" << height();

    reScale();
}

void GameBoard::reScale()
{
    qreal newScale = 0.0;

    int cols = m_model->tubesCount() / 2;
    if (cols * 2 < m_model->tubesCount())
        cols ++;

    int spaceX = 10;
    int spaceY = 120;
    int tubesWidth = cols * (80 + spaceX) + spaceX;
    int tubesHeight = 2 * (180 + spaceY);

    newScale = qMin(qreal(width() / tubesWidth), qreal(height() / tubesHeight));

    CtGlobal::

}

