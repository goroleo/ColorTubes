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
    Q_PROPERTY(bool hasMoves READ hasMoves NOTIFY movesChanged)
    Q_PROPERTY(int level READ level NOTIFY levelChanged)

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

    bool         isSolved();
    int          level();

signals:
    void         solved();
    void         movesChanged();
    void         levelChanged();

public slots:
    void         undoMove();
    void         startAgain();
    void         randomFill();
    void         solve();

private slots:
    void         onScaleChanged();
    void         onApplicationStateChanged();

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
