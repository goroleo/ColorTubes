#ifndef BOTTLELAYER_H
#define BOTTLELAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPixmap>

class TubeItem;

class BottleLayer : public QQuickPaintedItem
{
    Q_OBJECT
    friend class TubeItem;

public:
    explicit BottleLayer(TubeItem *parent = 0);
    ~BottleLayer();

    QString  source();
    quint8   sourceId();

    void     setSource(QString newSource);
    void     setSource(quint8 newSourceId);

    static const quint8 CT_BOTTLE_NONE  = 0;
    static const quint8 CT_BOTTLE_WHOLE = 1;
    static const quint8 CT_BOTTLE_FRONT = 2;
    static const quint8 CT_BOTTLE_BACK  = 3;

private slots:
    void     onScaleChanged();
    void     onAngleChanged();

private:
    void     paint(QPainter *painter) override;

    TubeItem * m_tube;  // parent tube for this layer

    qreal    m_startY = 0;
    quint8   m_sourceId = CT_BOTTLE_NONE;
    QPixmap  m_drawPixmap;
};

#endif // BOTTLELAYER_H
