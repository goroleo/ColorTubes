#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickItem>

class BoardModel;
class TubeItem;


class GameBoard : public QQuickItem
{
    Q_OBJECT

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();


    int maxChildrenZ();

signals:

private slots:
    void     onScaleChanged();


private:

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    qreal scale() const;
    void reScale();
    void placeTubes();

    BoardModel * m_model;

    QVector<TubeItem *> * m_tubes;

    int rows;
    int cols;

    qreal spaceX = 5.0;
    qreal spaceY = 120.0;
};

#endif // GAMEBOARD_H
