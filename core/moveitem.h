#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QtCore>

class BoardModel;

class MoveItem
{

public:

    struct MoveFields {
        quint8 color;
        quint8 count;
        quint8 tubeTo;
        quint8 tubeFrom;
    };

    union MoveData {
        MoveFields fields;
        quint32 stored;
    };

    MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo);
    MoveItem(quint32 storedMove);
    ~MoveItem();

    bool         doMove();

    bool operator < (const MoveItem & other) const {
        return m_rank < other.m_rank;
    }

    MoveItem   * parentMove()  { return m_parentMove; }
    BoardModel * boardBefore() { return m_boardBefore; }
    BoardModel * boardAfter()  { return m_boardAfter; }
    quint8       tubeFrom()    const { return m_data.fields.tubeFrom; }
    quint8       tubeTo()      const { return m_data.fields.tubeTo; }
    quint8       color()       const { return m_data.fields.color; }
    quint8       count()       const { return m_data.fields.count; }
    qint8        rank()        const { return m_rank;}
    quint32      stored()      { return m_data.stored; }

    void         setRank(qint8 rank) {m_rank = rank;}

private:

    MoveData     m_data;
    qint8        m_rank;

    MoveItem   * m_parentMove = nullptr;
    BoardModel * m_boardBefore = nullptr;
    BoardModel * m_boardAfter = nullptr;
};

QDebug operator << (QDebug dbg, const MoveItem & moveItem);
QDebug operator << (QDebug dbg, const MoveItem * moveItem);

class MoveItems: public QVector<MoveItem * >
{
public:
    void         sortByRank();
    MoveItem   * current();
    void         clear();
};

#endif // MOVEITEM_H
