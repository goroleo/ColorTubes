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
    int indexOf(TubeItem * tube);
    int tubesCount();

    void clickTube(TubeItem * tube);
    TubeItem * selectedTube() { return m_selectedTube;}

signals:

private slots:
    void     onScaleChanged();

private:

    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void mousePressEvent(QMouseEvent* event);
    qreal scale() const;
    void reScale();
    void placeTubes();
    void moveColor(TubeItem * tubeFrom, TubeItem * tubeTo);
    void moveColor(int tubeFromIndex, int tubeToIndex);


    BoardModel * m_model;

    QVector<TubeItem *> * m_tubes;

    int rows;
    int cols;

    qreal spaceX = 5.0;
    qreal spaceY = 120.0;

    TubeItem * m_selectedTube = nullptr;
};

#endif // GAMEBOARD_H
