#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickItem>

class BoardModel;


class GameBoard : public QQuickItem
{
    Q_OBJECT

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();

signals:


private:

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void reScale();

    BoardModel * m_model;
};

#endif // GAMEBOARD_H
