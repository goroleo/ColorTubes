#ifndef BOTTLELAYER_H
#define BOTTLELAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPixmap>

class TubeItem;

#define CT_BOTTLE_NONE       0
#define CT_BOTTLE_WHOLE      1
#define CT_BOTTLE_FRONT      2
#define CT_BOTTLE_BACK       3

class BottleLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource)

public:
    explicit BottleLayer(TubeItem *parent = 0);
    ~BottleLayer();

    void     paint(QPainter *painter) override;

    QString  source();
    quint8   sourceId();

public slots:
    void     setSource(QString newSource);
    void     setSource(quint8 newSourceId);

signals:
//    void     sourceChanged();

private slots:
    void     onScaleChanged();
    void     onAngleChanged();

private:
    TubeItem * parentTube;

    qreal    scale();
    qreal    dy = 0;
    quint8   m_source_id = CT_BOTTLE_NONE;
    qreal    m_angle = 0.0;
    QPixmap  m_drawImage;
};

#endif // BOTTLELAYER_H
