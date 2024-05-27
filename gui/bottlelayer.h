#ifndef BOTTLELAYER_H
#define BOTTLELAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPixmap>

class TubeItem;

class BottleLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource)

    friend class TubeItem;

public:
    explicit BottleLayer(TubeItem *parent = 0);
    ~BottleLayer();

    void     paint(QPainter *painter) override;

    QString  source();
    quint8   sourceId();

    static const quint8 CT_BOTTLE_NONE  = 0;
    static const quint8 CT_BOTTLE_WHOLE = 1;
    static const quint8 CT_BOTTLE_FRONT = 2;
    static const quint8 CT_BOTTLE_BACK  = 3;

public slots:
    void     setSource(QString newSource);
    void     setSource(quint8 newSourceId);

private slots:
    void     onScaleChanged();
    void     onAngleChanged();

private:
    TubeItem * m_tube;

    qreal    m_startY = 0;
    quint8   m_source_id = CT_BOTTLE_NONE;
    QPixmap  m_drawPixMap;

};

#endif // BOTTLELAYER_H
