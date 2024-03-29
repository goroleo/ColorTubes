#include "corklayer.h"

#include <QPainter>
#include <QBrush>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/tubeimages.h"
#include "src/palette.h"

CorkLayer::CorkLayer(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    internalTimer = new QTimer(this);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    connect(internalTimer, &QTimer::timeout, [=](){
        nextStep();
        prepareImage();
        update();
    });

    onScaleChanged();
}

CorkLayer::~CorkLayer()
{
    delete internalTimer;
}

bool CorkLayer::isVisible()
{
    return m_visible;
}

void CorkLayer::setVisible(bool newVisible)
{
    if (m_visible == newVisible)
        return;
    if (newVisible)
        startShow();
    else
        startHide();
    emit visibleChanged(m_visible);
}

void CorkLayer::startShow()
{
    m_visible = true;
    m_alphaIncrement = ALPHA_INC_UP;
    if (!internalTimer->isActive()) {
        internalTimer->start(TIMER_TICKS);
    }
}

void CorkLayer::startHide()
{
    m_visible = false;
    m_alphaIncrement = ALPHA_INC_DOWN;
    if (!internalTimer->isActive()) {
        internalTimer->start(TIMER_TICKS);
    }
}

void CorkLayer::onScaleChanged()
{
    m_drawImage = QImage(CtGlobal::images().cork().width(),
                         CtGlobal::images().cork().height(),
                         QImage::Format_ARGB32);

    setWidth(m_drawImage.width());
    setHeight(m_drawImage.height() + 20 * CtGlobal::images().scale());

    m_drawImage.fill(0x00ffffff);
    prepareImage();
    update();
}

void CorkLayer::paint(QPainter *painter)
{
    painter->drawImage(0, m_currentY, m_drawImage);
}

void CorkLayer::nextStep()
{
    m_alpha += m_alphaIncrement;

    if (m_visible) {
        if (m_alpha > 1) {
            m_alpha = 1.0;
            internalTimer->stop();
        }
    } else {
        if (m_alpha < 0) {
            m_alpha = 0.0;
            internalTimer->stop();
        }
    }

    m_currentY = - 20 * CtGlobal::images().scale()
            + 40 * CtGlobal::images().scale() * exp(-0.5 * (1 - m_alpha));
}

void CorkLayer::prepareImage()
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
