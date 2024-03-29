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

    int          tubesCount();
    int          indexOf(TubeItem * tube);
    void         clickTube(TubeItem * tube);
    TubeItem   * selectedTube() { return m_selectedTube;}
    int          maxChildrenZ();
    bool         checkSolved();
    bool         isSolved() { return m_solved;}
    void         showAvailableMoves();

signals:

private slots:
    void         onScaleChanged();

private:
    void         geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void         mousePressEvent(QMouseEvent* event);
    qreal        scale() const;
    void         placeTubes();

    void         moveColor(TubeItem * tubeFrom, TubeItem * tubeTo);
    void         moveColor(int tubeFromIndex, int tubeToIndex);

    BoardModel * m_model;
    qreal        m_scale;
    bool         m_solved = false;

    QVector<TubeItem *>
               * m_tubes;

    int          rows;
    int          cols;

    qreal        spaceX = 5.0;
    qreal        spaceY = 100.0;

    TubeItem   * m_selectedTube = nullptr;
};

#endif // GAMEBOARD_H
