#ifndef TUBEMODEL_H
#define TUBEMODEL_H

#include <QtCore>

class TubeModel
{
public:
    TubeModel();
    ~TubeModel();

    void clear();

    quint8 count() const {return m_count;}
    quint8 rest() const {return 4 - m_count;}
    quint8 state() const {return m_state;}

    // state routines
    bool isClosed() const;
    bool isEmpty() const;
    void updateState();

    //  get color(s)
    quint8 currentColor() const;
    quint8 color(quint8 index);
    bool hasColor(quint8 colorNumber) const;

    //  put color
    bool canPutColor(quint8 colorNumber) const;
    bool putColor(quint8 colorNumber);

    //  extract color(s)
    bool canExtractColor() const;
    quint8 extractColor();
    quint8 sameColorCount() const;

    //  compare to another tube
    bool operator == (const TubeModel & other);

    // store & restore
    void assignColors(quint32 storedTube);
    void assignColors(TubeModel * other);
    quint32 store() const;

    static const quint8 STATE_EMPTY   = 0;
    static const quint8 STATE_REGULAR = 1;
    static const quint8 STATE_FILLED  = 2;
    static const quint8 STATE_CLOSED  = 3;

private:
    bool checkClosed();
    bool putColor(quint8 colorNum, bool updateState);

    union TubeCells {
        quint8 items[4];
        quint32 stored;
    };

    TubeCells m_colors;
    int m_state;
    int m_count;
};

#endif // TUBEMODEL_H
