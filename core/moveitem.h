#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QtCore>

class BoardModel;

class MoveItem
{

public:

    struct MoveFields {
        quint8 tubeFrom;
        quint8 tubeTo;
        quint8 count;
        quint8 color;
    };

    union MoveData {
        MoveFields fields;
        quint32 stored;
    };

    MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo);
    ~MoveItem();

    bool         doMove();
    bool         undoMove();
    quint32      stored();
//    MoveFields   data();

    bool operator < (const MoveItem & other) const {
        return m_rank < other.m_rank;
    }

    MoveItem   * parentMove()  { return m_parentMove; }
    BoardModel * boardBefore() { return m_boardBefore; }
    BoardModel * boardAfter()  { return m_boardAfter; }
    quint8       tubeFrom()    { return m_data.fields.tubeFrom; }
    quint8       tubeTo()      { return m_data.fields.tubeTo; }
    quint8       color()       { return m_data.fields.color; }
    quint8       count()       { return m_data.fields.count; }
    qint8        rank()        { return m_rank;}

private:

    MoveData     m_data;
    qint8        m_rank;

    MoveItem   * m_parentMove = nullptr;
    BoardModel * m_boardBefore = nullptr;
    BoardModel * m_boardAfter = nullptr;
};

class GameMoves: public QVector<MoveItem*>{};

#endif // MOVEITEM_H
