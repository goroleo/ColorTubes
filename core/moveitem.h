#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QtCore>

class BoardModel;

class MoveItem
{

public:

    MoveItem();
    MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo);
    ~MoveItem();

    bool         doMove();
    bool         undoMove();
    quint32      storeMove();

    bool operator < (const MoveItem & other) const {
        return m_rank < other.m_rank;
    }

    void         setParentMove(MoveItem * move);
    void         setBoardBefore(BoardModel * board);
    void         setTubeFrom(quint8 idxTubeFrom);
    void         setTubeTo(quint8 idxTubeTo);
    void         setRank(qint8 newRank);

    const MoveItem   * parentMove()  { return m_parentMove; }
    const BoardModel * boardBefore() { return m_boardBefore; }
    BoardModel * boardAfter()  { return m_boardAfter; }
    quint8       tubeFrom()    { return m_tubeFrom; }
    quint8       tubeTo()      { return m_tubeTo; }
    qint8        rank()        { return m_rank;}

private:

    quint8       m_tubeFrom;
    quint8       m_tubeTo;
    quint8       m_color;
    quint8       m_count;
    qint8        m_rank;

    const MoveItem   * m_parentMove = nullptr;
    const BoardModel * m_boardBefore = nullptr;
    BoardModel * m_boardAfter = nullptr;
};

#endif // MOVEITEM_H
