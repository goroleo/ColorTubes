#include "flowerlayer.h"
#include <qapplication.h>

FlowerLayer::FlowerLayer(QQuickItem * parent) :
    QQuickPaintedItem(parent)
{
    if (parent) {
        m_edgeSize = qMax(parent->width(), parent->height());
        setWidth(parent->width());
        setHeight(parent->height());
        prepareImage();
    } else
        m_edgeSize = 0;
    m_source = new QSvgRenderer(QLatin1String(":/img/flower.svg"));
}

FlowerLayer::~FlowerLayer()
{
    stopRotate();
    if (m_painter)
        delete m_painter;
    if (m_drawImage)
        delete m_drawImage;
    delete m_source;
}

void FlowerLayer::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
    m_edgeSize = qMax(newGeometry.width(), newGeometry.height());
    prepareImage();
}

void FlowerLayer::prepareImage()
{
    if (qFuzzyIsNull(m_edgeSize))
        return;

    bool oldRotated = rotated;
    stopRotate();

    if (m_painter)
        delete m_painter;
    if (m_drawImage)
        delete m_drawImage;

    m_drawImage = new QImage(width(), height(), QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    qreal scale;
    if (width() < m_edgeSize) {
        scale = m_edgeSize / m_source->defaultSize().rheight();
        m_source->setViewBox(
                    QRectF(
                        (m_edgeSize - width()) / 2 / scale,
                        0,
                        m_source->defaultSize().rwidth() - (m_edgeSize - width()) / scale,
                        m_source->defaultSize().rheight()));
    } else {
        scale = m_edgeSize / m_source->defaultSize().rwidth();
        m_source->setViewBox(
                    QRectF(
                        0,
                        (m_edgeSize - height()) / 2 / scale,
                        m_source->defaultSize().rwidth(),
                        m_source->defaultSize().rheight() - (m_edgeSize - height()) / scale));
    }

    if (oldRotated)
        startRotate();
    else {
        m_drawImage->fill(0);
        m_source->render(m_painter);
    }
}

void FlowerLayer::paint(QPainter * painter)
{
    painter->drawImage(0, 0, * m_drawImage);
}

void FlowerLayer::repaintFrame()
{
    if (QApplication::applicationState() == Qt::ApplicationActive) {
        m_drawImage->fill(0);
        m_source->render(m_painter);
        update();
    }
}

void FlowerLayer::startRotate()
{
    if (!rotated && m_painter) {
        QObject::connect(m_source, &QSvgRenderer::repaintNeeded,
                         this, &FlowerLayer::repaintFrame);
        rotated = true;
    }
}

void FlowerLayer::stopRotate()
{
    if (rotated) {
        QObject::disconnect(m_source, &QSvgRenderer::repaintNeeded,
                            this, &FlowerLayer::repaintFrame);
        rotated = false;
    }
}
