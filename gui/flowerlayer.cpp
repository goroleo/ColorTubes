#include "flowerlayer.h"
#include <qguiapplication.h>

FlowerLayer::FlowerLayer(QQuickItem * parent) :
    QQuickPaintedItem(parent)
{
    if (parent) {
        m_edgeSize = qMax(parent->width(), parent->height());
        setWidth(parent->width());
        setHeight(parent->height());
        prepareImage();
        QObject::connect(parentItem(), &QQuickItem::opacityChanged,
                this, &FlowerLayer::onOpacityChanged);
    } else
        m_edgeSize = 0;
    m_source = new QSvgRenderer(QLatin1String(":/img/flower.svg"));

    QObject::connect(qGuiApp, &QGuiApplication::applicationStateChanged,
            this, &FlowerLayer::onApplicationStateChanged);

}

FlowerLayer::~FlowerLayer()
{
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

    if (parentItem()) {
        QObject::connect(parentItem(), &QQuickItem::opacityChanged,
                         this, &FlowerLayer::onOpacityChanged);
    }
}

void FlowerLayer::onOpacityChanged()
{
    if (parentItem() && qFuzzyIsNull(parentItem()->opacity()))
        disconnectFromSvg();
    else
        if (!svgConnected)
            connectToSvg();
}

void FlowerLayer::onApplicationStateChanged()
{
    if (QGuiApplication::applicationState() != Qt::ApplicationActive)
        m_source->blockSignals(true);
    else
        m_source->blockSignals(false);
}

void FlowerLayer::prepareImage()
{
    if (qFuzzyIsNull(m_edgeSize))
        return;

    bool oldConnected = svgConnected;
    disconnectFromSvg();

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

    if (oldConnected)
        connectToSvg();
    else {
        m_drawImage->fill(0);
        m_source->render(m_painter);
    }
}

void FlowerLayer::paint(QPainter * painter)
{
    painter->drawImage(0, 0, * m_drawImage);
}

void FlowerLayer::paintFrame()
{
    m_drawImage->fill(0);
    m_source->render(m_painter);
    update();
}

void FlowerLayer::connectToSvg()
{
    if (!svgConnected && m_painter) {
        QObject::connect(m_source, &QSvgRenderer::repaintNeeded,
                         this, &FlowerLayer::paintFrame);
        svgConnected = true;
    }
}

void FlowerLayer::disconnectFromSvg()
{
    if (svgConnected) {
        QObject::disconnect(m_source, &QSvgRenderer::repaintNeeded,
                            this, &FlowerLayer::paintFrame);
        svgConnected = false;
    }
}
