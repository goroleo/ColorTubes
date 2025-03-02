#ifndef FLOWERLAYER_H
#define FLOWERLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>
#include "src/ctglobal.h"

class FlowerLayer: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit     FlowerLayer(QQuickItem *parent = 0);
    qreal        opacity() {return m_opacity;}
    ~FlowerLayer();

private slots:
    void         onParentOpacityChanged();
    void         onApplicationStateChanged();

private:
    QSvgRenderer * m_source;
    QImage       * m_frameImage = nullptr;
    QImage       * m_sourceImage = nullptr;
    QTimer       * m_timer = nullptr;

    qreal        m_opacity = 1.0;
    void         setOpacity(qreal newOpacity);
    void         applyOpacity();

    qreal        m_angle;
    qreal        m_sectorAngle = CT_DEG2RAD * 20.0;

    void         nextFrame();
    void         paintFrame();
    void         geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void         paint(QPainter * painter) override;

    qreal        m_sourceSize = 0.0;
    int          m_sourceCenter;
    QRect        m_bounds;
    QPoint       m_shift;
    qreal        m_sin;
    qreal        m_cos;
    bool         m_parentConnected = false;
};

#endif // FLOWERLAYER_H
