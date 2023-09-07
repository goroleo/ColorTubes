#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QtCore>

class BoardModel;

class MoveItem
{

public:
    MoveItem();
    MoveItem(BoardModel *board, quint8 idxTubeFrom, quint8 idxTubeTo);
    ~MoveItem();

    bool doMove();

    bool operator < (const MoveItem &other) const {
        return m_rank < other.m_rank;
    }

    void setParentMove(MoveItem *item);
    void setBoardBefore(BoardModel *bm);
    void setTubeFrom(quint8 idxTubeFrom);
    void setTubeTo(quint8 idxTubeTo);
    void setRank(qint8 newRank);

    MoveItem *parentMove() { return m_parent; }
    BoardModel *boardBefore() { return m_boardBefore; }
    BoardModel *boardAfter() { return m_boardAfter; }
    quint8 tubeFrom() { return m_tubeFrom; }
    quint8 tubeTo() { return m_tubeTo; }
    qint8 rank() {return m_rank;}

    quint32 stored();

private:

    quint8     m_tubeFrom;
    quint8     m_tubeTo;
    quint8     m_color;
    quint8     m_count;
    qint8      m_rank;

    MoveItem   *m_parent = nullptr;
    BoardModel *m_boardBefore = nullptr;
    BoardModel *m_boardAfter = nullptr;

};

#endif // MOVEITEM_H
