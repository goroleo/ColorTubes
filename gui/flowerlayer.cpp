#include "flowerlayer.h"
#include <qguiapplication.h>
#include <QTimer>
#include "src/ctio.h"


FlowerLayer::FlowerLayer(QQuickItem * parent) :
    QQuickPaintedItem(parent)
{
    m_source = new QSvgRenderer(QLatin1String(":/img/flower.svg"));

//    QObject::connect(qGuiApp, &QGuiApplication::applicationStateChanged,
//            this, &FlowerLayer::onApplicationStateChanged);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=](){
        nextFrame();
        paintFrame();
        update();
    });
}

FlowerLayer::~FlowerLayer()
{
    delete m_timer;
    delete m_source;

    if (m_sourceImage)
        delete m_sourceImage;
    if (m_frameImage)
        delete m_frameImage;
}

void FlowerLayer::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);

    m_bounds = QRect(newGeometry.x(), newGeometry.y(),
                     newGeometry.width(), newGeometry.height());
    m_sourceSize = qMax(newGeometry.width(), newGeometry.height()) * 1.1;
    m_sourceCenter = qRound(m_sourceSize / 2);
    m_shift = QPoint((m_sourceSize - m_bounds.width()) /2,
                     (m_sourceSize - m_bounds.height()) /2);

    if (m_sourceImage)
        delete m_sourceImage;
    m_sourceImage = new QImage(m_sourceSize, m_sourceSize,
                               QImage::Format_ARGB32);
    applyOpacity();

    if (m_frameImage)
        delete m_frameImage;
    m_frameImage = new QImage(m_bounds.width(),
                             m_bounds.height(),
                             QImage::Format_ARGB32);
    paintFrame();

    if (!m_parentConnected && parentItem()) {
        QObject::connect(parentItem(), &QQuickItem::opacityChanged,
                         this, &FlowerLayer::onParentOpacityChanged);
        m_parentConnected = true;
    }
}

void FlowerLayer::setOpacity(qreal newOpacity)
{
    if (qFuzzyCompare(m_opacity, newOpacity))
        return;
    m_opacity = newOpacity;
    applyOpacity();
    if (m_frameImage)
        paintFrame();
}

void FlowerLayer::applyOpacity()
{
    if (!m_sourceImage)
        return;

    m_sourceImage->fill(0x00ffffff);
    if (qFuzzyIsNull(m_opacity))
        return;

    QPainter sourcePainter(m_sourceImage);
    QRectF sourceBounds = QRectF(0, 0, m_sourceSize, m_sourceSize);
    m_source->render(&sourcePainter, sourceBounds);

    if (qFuzzyCompare(m_opacity, qreal(1.0)))
        return;

    for (int x = 0; x < m_sourceSize; ++x)
        for (int y = 0; y < m_sourceSize; ++y) {
            quint32 pix = m_sourceImage->pixel(x,y);
            quint8 alpha = (pix >> 24) & 0xff;
            if (alpha > 0) {
                pix = (pix & 0xffffff) | (qRound(m_opacity * qreal(alpha)) << 24);
                m_sourceImage->setPixel(x, y, pix);
            }
        }
}

void FlowerLayer::onParentOpacityChanged()
{
/*
    if (parentItem() && !qFuzzyIsNull(parentItem()->opacity())) {
        setOpacity(parentItem()->opacity());
        if (!m_timer->isActive())
            m_timer->start(CT_FLOWER_TICKS);
    } else {
        m_timer->stop();
    }
*/

//    if (parentItem())
//        setOpacity(parentItem()->opacity());
}

/*
void FlowerLayer::onApplicationStateChanged()
{
    if (QGuiApplication::applicationState() == Qt::ApplicationActive) {
        if (!qFuzzyIsNull(m_opacity)
                && (parentItem() && !qFuzzyIsNull(parentItem()->opacity()))
                && !m_timer->isActive()) {
//            m_timer->start(CT_FLOWER_TICKS);
        }
    } else {
//        m_timer->stop();
    }
}
*/

void FlowerLayer::paint(QPainter * painter)
{
    painter->drawImage(0, 0, * m_frameImage);
}

void FlowerLayer::nextFrame()
{
    m_angle += CT_FLOWER_ANGLE_INC;
    while (m_angle > m_sectorAngle)
        m_angle -= m_sectorAngle;
    m_sin = qSin(m_angle);
    m_cos = qCos(m_angle);
}

void FlowerLayer::paintFrame()
{
    m_frameImage->fill(0);

//    qreal parentOpacity = parentItem() ? parentItem()->opacity() : 0.0;
//    if (qFuzzyIsNull(parentOpacity))
//        return;

    for (int x = 0; x < m_bounds.width(); ++x)
        for (int y = 0; y < m_bounds.height(); ++y) {

            int dx = x + m_shift.x() - m_sourceCenter;
            int dy = y + m_shift.y() - m_sourceCenter;
            int sourceX = m_sourceCenter + qRound(m_cos * qreal(dx) + m_sin * qreal(dy));
            int sourceY = m_sourceCenter + qRound(m_cos * qreal(dy) - m_sin * qreal(dx));

            if ((sourceX >= 0 && sourceX < m_sourceSize)
                    && (sourceY >= 0 && sourceY < m_sourceSize)) {

                quint32 pix = m_sourceImage->pixel(sourceX, sourceY);
                m_frameImage->setPixel(x, y, pix);

//                quint8 alpha = (pix >> 24) & 0xff;
//                if (alpha > 0) {
//                    pix = (pix & 0xffffff) | (qRound(parentOpacity * qreal(alpha)) << 24);
//                    m_frameImage->setPixel(x, y, pix);
//                }
            }
        }
}

