﻿#ifndef FLOWERLAYER_H
#define FLOWERLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QImage>
#include <QPainter>
#include <QSvgRenderer>

class FlowerLayer: public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit     FlowerLayer(QQuickItem *parent = 0);
    ~FlowerLayer();

private slots:
    void         onOpacityChanged();
    void         onApplicationStateChanged();

private:
    QSvgRenderer * m_source;
    QImage       * m_drawImage = nullptr;
    QPainter     * m_painter = nullptr;

    qreal        m_edgeSize;
    bool         svgConnected = false;
    void         connectToSvg();
    void         disconnectFromSvg();
    void         prepareImage();
    void         paintFrame();
    void         geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void         paint(QPainter * painter) override;
};

#endif // FLOWERLAYER_H
