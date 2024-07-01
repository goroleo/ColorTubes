#ifndef TUBEMODEL_H
#define TUBEMODEL_H

#include <QtCore>

class TubeModel : public QObject
{
    Q_OBJECT

public:

    TubeModel();
    ~TubeModel();

    void clear();

    quint8 count() const {return m_count;}
    quint8 rest() const {return 4 - m_count;}
    quint8 state() const {return m_state;}

    // state routines
    bool isDone() const;
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
    void assignColors(quint32 storedColors);
    void assignColors(TubeModel * other);
    quint32 store() const;

signals:
    void stateChanged();

private:
    bool checkDone();
    bool putColor(quint8 colorNum, bool updateState);

    union ColorCells {
        quint8 items[4];
        quint32 stored;
    };

    ColorCells m_colors;
    quint8 m_state;
    quint8 m_count;
};

class TubeModels: public QVector<TubeModel*>{};

#endif // TUBEMODEL_H
