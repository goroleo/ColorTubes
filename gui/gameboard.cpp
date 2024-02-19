#include "gameboard.h"

GameBoard::GameBoard(QQuickItem *parent) :
    QQuickItem(parent)
{

}


GameBoard::~GameBoard()
{

}

void GameBoard::ready()
{
    qDebug() << "width" << width() << "| height" << height();
}

void GameBoard::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    qDebug() << "oldWidth" << oldGeometry.width() << "| oldHeight" << oldGeometry.height();
    qDebug() << "newWidth" << newGeometry.width() << "| newHeight" << newGeometry.height();
    qDebug() << "curWidth" << width() << "| curHeight" << height();
}
