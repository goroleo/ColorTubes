#ifndef MOVEITEM_H
#define MOVEITEM_H

#include <QtCore>

class BoardModel;
class MoveItems;

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

class MoveItem
{

public:

    MoveItem(BoardModel * board, quint8 idxTubeFrom, quint8 idxTubeTo);
    MoveItem(quint32 storedMove);
    ~MoveItem();

    bool         doMove();

    MoveItem   * parent()      { return m_parentMove; }
    BoardModel * boardBefore() { return m_boardBefore; }
    BoardModel * boardAfter()  { return m_boardAfter; }
    quint8       tubeFrom()    const { return m_data.fields.tubeFrom; }
    quint8       tubeTo()      const { return m_data.fields.tubeTo; }
    quint8       color()       const { return m_data.fields.color; }
    quint8       count()       const { return m_data.fields.count; }
    quint32      stored()      { return m_data.stored; }

    qint8        rank;

    MoveItems  * children()     { return m_children; }
    int          childrenCount();
    MoveItem   * currentChild();
    bool         hasChildren();
    void         addChild(quint32 storedMove);
    void         addChild(MoveItem * move);
    void         removeLastChild();

private:

    MoveData     m_data;

    MoveItem   * m_parentMove = nullptr;
    BoardModel * m_boardBefore = nullptr;
    BoardModel * m_boardAfter = nullptr;
    MoveItems  * m_children = nullptr;
};

QDebug operator << (QDebug dbg, const MoveItem & moveItem);
QDebug operator << (QDebug dbg, const MoveItem * moveItem);

class MoveItems: public QVector<MoveItem *>
{
public:
    ~MoveItems();
    void         sortByRank();
    MoveItem   * current();
    void         removeLast();
    void         clear();
};

#endif // MOVEITEM_H
