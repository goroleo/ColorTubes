#ifndef BOTTLELAYER_H
#define BOTTLELAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPixmap>

class TubeItem;

class BottleLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

public:
    explicit BottleLayer(TubeItem *parent = 0);
    ~BottleLayer();

    void paint(QPainter *painter) override;

    QString source();

public slots:
    void setSource(QString newSource);

signals:
    void sourceChanged(const int newShadeNumber);

private slots:
    void onScaleChanged();
    void onAngleChanged();

private:
    TubeItem * parentTube;

    bool findImage(QString aSource);
    qreal scale();
    qreal dy = 0;
    QString  m_source;
    qreal    m_angle = 0.0;
    QPixmap  m_drawImage;
};

#endif // BOTTLELAYER_H
