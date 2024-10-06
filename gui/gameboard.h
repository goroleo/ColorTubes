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

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();

    int          tubesCount();
    int          indexOf(TubeItem * tube);
    TubeItem   * selectedTube() { return m_selectedTube; }

    bool         hasMoves();
    void         showAvailableMoves();

    void         clickTube(TubeItem * tube);
    int          maxChildrenZ();
    void         update();
    void         refresh();

    bool         busy();
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
    void         rescale();

    void         placeTubes();

    TubeItems  * m_tubes;
    TubeItem   * m_selectedTube = nullptr;

    qreal        spaceX = 5.0;
    qreal        spaceY = 100.0;
};

#endif // GAMEBOARD_H
