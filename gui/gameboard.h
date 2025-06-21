#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickItem>

class BoardModel;
class MoveItems;
class TubeItem;
class TubeItems;
class ArrowItem;

class GameBoard : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    friend class TubeItem;

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();

    int          indexOf(TubeItem * tube);
    void         clickTube(TubeItem * tube);
    TubeItem   * selectedTube() { return m_selectedTube; }

    Q_INVOKABLE void hideSelection();
    void         showMove();
    void         hideMove();

signals:
    void         busyChanged();

private slots:
    void         onScaleChanged();
    void         onLevelChanged();
    void         onRefresh();
    void         onArrowInShown();
    void         onArrowOutShown();
    void         onApplicationStateChanged();

private:
    BoardModel * m_model;

    void         geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void         mousePressEvent(QMouseEvent* event);
    void         placeTubes();
    void         rescale();
    void         update();
    void         newMove(TubeItem *from, TubeItem *to);

    TubeItems  * m_tubes;
    TubeItem   * m_selectedTube = nullptr;

    ArrowItem  * m_arrowIn;
    ArrowItem  * m_arrowOut;

    qreal        spaceX = 5.0;
    qreal        spaceY = 100.0;

    int          maxChildrenZ();  // maximal Z coordinate of all children TubeItems
    void         childrenZChanged();
    bool         busy() { return m_busy; };
    bool         m_busy;

};

#endif // GAMEBOARD_H
