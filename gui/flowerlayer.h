#ifndef FLOWERLAYER_H
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
    explicit FlowerLayer(QQuickItem *parent = 0);
    ~FlowerLayer();

public slots:
    void startRotate();
    void stopRotate();

private:
    QSvgRenderer * m_source;
    QImage    * m_drawImage = nullptr;
    QPainter  * m_painter = nullptr;

    qreal m_edgeSize;
    bool rotated = false;
    void prepareImage();
    void repaintFrame();
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
    void paint(QPainter * painter) override;
};

#endif // FLOWERLAYER_H
