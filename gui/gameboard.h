#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickItem>

class GameBoard : public QQuickItem
{
    Q_OBJECT
public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();


    void ready();
//    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

signals:


private:

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

};

#endif // GAMEBOARD_H
