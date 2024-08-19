#include "corklayer.h"

#include <QPainter>
#include <QBrush>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/ctimages.h"
#include "src/ctpalette.h"

CorkLayer::CorkLayer(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    m_timer = new QTimer(this);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    connect(m_timer, &QTimer::timeout, [=](){
        nextFrame();
        paintFrame();
        update();
    });

    onScaleChanged();
}

CorkLayer::~CorkLayer()
{
    delete m_timer;
}

bool CorkLayer::isVisible()
{
    return m_visible;
}

void CorkLayer::setVisible(bool value)
{
    if (m_visible == value)
        return;
    if (value)
        startShow();
    else
        startHide();
}

void CorkLayer::setAnimated(bool value)
{
    if (m_animated == value)
        return;
    m_animated = value;
    if (m_visible)
        startShow();
    else
        startHide();
}

void CorkLayer::startShow()
{
    m_visible = true;
    if (m_animated) {
        m_alphaIncrement = 1.0 / qreal(CT_CORK_STEPS_INC);
        if (!m_timer->isActive()) {
            m_timer->start(CT_TIMER_TICKS);
        }
    } else {
        m_alpha = 1.1;
        nextFrame();
        paintFrame();
        update();
    }
}

void CorkLayer::startHide()
{
    m_visible = false;
    if (m_animated) {
        m_alphaIncrement = -1.0 / qreal(CT_CORK_STEPS_DEC);
        if (!m_timer->isActive()) {
            m_timer->start(CT_TIMER_TICKS);
        }
    } else {
        m_alpha = -0.1;
        nextFrame();
        paintFrame();
        update();
    }
}

void CorkLayer::onScaleChanged()
{
    m_drawImage = QImage(CtGlobal::images().cork().width(),
                         CtGlobal::images().cork().height(),
                         QImage::Format_ARGB32);

    setWidth(m_drawImage.width());
    setHeight(m_drawImage.height() + CtGlobal::images().shiftHeight());

    m_drawImage.fill(0x00ffffff);
    paintFrame();
    update();
}

void CorkLayer::paint(QPainter * painter)
{
    painter->drawImage(0, m_currentY, m_drawImage);
}

void CorkLayer::nextFrame()
{
    m_alpha += m_alphaIncrement;

    if (m_visible) {
        if (m_alpha > 1) {
            m_alpha = 1.0;
            m_timer->stop();
        }
    } else {
        if (m_alpha < 0) {
            m_alpha = 0.0;
            m_timer->stop();
        }
    }

    m_currentY = - CtGlobal::images().shiftHeight()
            + CtGlobal::images().shiftHeight() * 2 * exp(-0.5 * (1 - m_alpha));
}

void CorkLayer::paintFrame()
{
    quint32 pix;
    int newAlpha;
    QImage corkImage = CtGlobal::images().cork().toImage();

    for (int x = 0; x < corkImage.width(); ++x) {
        for (int y = 0; y < corkImage.height(); ++y) {
            pix = corkImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = round (qreal(pix >> 24) * m_alpha);
                m_drawImage.setPixel(x, y, ((newAlpha & 0xff) << 24) | (pix & 0xffffff));
            }
        }
    }
}
