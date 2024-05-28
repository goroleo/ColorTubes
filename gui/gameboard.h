#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QQuickItem>

class BoardModel;
class GameMoves;
class TubeItem;
class TubeItems;

class GameBoard : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool hasMoves READ hasMoves NOTIFY movesChanged)

public:
    explicit GameBoard(QQuickItem *parent = nullptr);
    ~GameBoard();

    int          tubesCount();
    int          indexOf(TubeItem * tube);
    TubeItem   * selectedTube() { return m_selectedTube; }

    GameMoves  * moves();
    void         addNewMove(TubeItem * tubeFrom, TubeItem * tubeTo);
    bool         hasMoves();
    void         showAvailableMoves();

    void         clickTube(TubeItem * tube);
    int          maxChildrenZ();

    bool         isSolved();

signals:
    void         solved();
    void         movesChanged();

public slots:
    void         undoMove();
    void         startAgain();

private slots:
    void         onScaleChanged();

private:
    BoardModel * m_model;

    void         geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void         mousePressEvent(QMouseEvent* event);
    qreal        scale() const;
    void         placeTubes();

    TubeItems  * m_tubes;
    TubeItem   * m_selectedTube = nullptr;

    qreal        spaceX = 5.0;
    qreal        spaceY = 100.0;
};

#endif // GAMEBOARD_H
