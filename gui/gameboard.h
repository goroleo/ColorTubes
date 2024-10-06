#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickItem>

class BoardModel;
class MoveItems;
class TubeItem;
class TubeItems;

class GameBoard : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

    friend class TubeItem;

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();

    int          tubesCount();
    int          indexOf(TubeItem * tube);
    void         clickTube(TubeItem * tube);
    TubeItem   * selectedTube() { return m_selectedTube; }
    Q_INVOKABLE void hideSelection();

signals:
    void         busyChanged();

private slots:
    void         onScaleChanged();
    void         onLevelChanged();
    void         onRefresh();

private:
    BoardModel * m_model;

    void         geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void         mousePressEvent(QMouseEvent* event);
    int          maxZ();  // maximal Z coordinate of all children TubeItems
    void         placeTubes();
    void         rescale();
    void         update();
    bool         busy();

    TubeItems  * m_tubes;
    TubeItem   * m_selectedTube = nullptr;

    qreal        spaceX = 5.0;
    qreal        spaceY = 100.0;
};

#endif // GAMEBOARD_H
