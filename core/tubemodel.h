#ifndef TUBEMODEL_H
#define TUBEMODEL_H

#include <QtCore>
#include "src/ctglobal.h"

union ColorCells {
    quint8 items[4];
    quint32 stored;
};

class TubeModel : public QObject
{
    Q_OBJECT

public:

    explicit TubeModel();
    ~TubeModel();

    void clear();

    quint8 count() const {return m_count;}
    quint8 rest() const {return 4 - m_count;}
    quint8 state() const {return m_state;}

    // state routines
    bool isDone() const;
    bool isEmpty() const;
    void updateState();

    // get color(s)
    quint8 currentColor() const;
    quint8 currentColorCount() const;     // how much colors we can get from the tube on one move
    quint8 color(quint8 index);
    bool hasColor(quint8 color) const;

    // put color
    bool canPutColor(quint8 color) const;
    bool putColor(quint8 color);

    // extract color(s)
    bool canExtractColor() const;
    quint8 extractColor();

    // compare to another tube
    bool operator == (const TubeModel &other);

    // store & restore
    void assignColors(quint32 storedColors);
    void assignColors(const TubeModel &tubeFrom);
    quint32 store() const;

signals:
    void stateChanged();

private:
    bool checkDone();
    bool putColor(quint8 color, bool updateState);

    ColorCells m_colors;
    quint8 m_state = CT_STATE_EMPTY;
    quint8 m_count = 0;
};

class TubeModels: public QVector<TubeModel*>
{
public:
    ~TubeModels();
    void         removeLast();
    void         clear();
};

#endif // TUBEMODEL_H
