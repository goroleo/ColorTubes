#ifndef TUBEMODEL_H
#define TUBEMODEL_H

#include <QtCore>

union TubeCells {
    quint8 colors[4];
    quint32 stored;
};

class TubeModel
{
public:
    TubeModel();
    ~TubeModel();

    void clear();

    quint8 count() const {return m_count;}
    quint8 rest() const {return 4 - m_count;}
    quint8 state() {return m_state;}

    // state routines
    bool isClosed();
    bool isEmpty();
    void updateState();

    //  get color(s)
    quint8 currentColor() const;
    quint8 color(quint8 index);
    bool hasColor(quint8 colorNumber);

    //  put color
    bool canPutColor(quint8 colorNumber);
    bool putColor(quint8 colorNumber);

    //  extract color(s)
    bool canExtractColor();
    quint8 extractColor();
    quint8 sameColorsCount() const;

    //  compare to another tube
    bool operator == (const TubeModel & other) const;

    // store & restore
    void assignColors(quint32 stored);
    void assignColors(TubeModel * tmFrom);
    quint32 storeColors() const;

private:
    bool checkClosed();
    bool putColor(quint8 colorNum, bool updateState);

    TubeCells m_items;
    int m_state;
    int m_count;
    bool m_needStateUpdate = false;
};

#endif // TUBEMODEL_H
