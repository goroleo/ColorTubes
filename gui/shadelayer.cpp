#include "shadelayer.h"

#include <QPixmap>
#include <QtMath>
#include <QtDebug>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

ShadeLayer::ShadeLayer(QQuickItem *parent) :
      QQuickPaintedItem(parent),
      m_shadeNumber(1)
{

    m_drawImage = QImage(80 * CtGlobal::images().scale(),
                         180* CtGlobal::images().scale(),
                         QImage::Format_ARGB32);

    setWidth(CtGlobal::images().width());
    setHeight(CtGlobal::images().height());

    m_drawImage.fill(0x00ffffff);


    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));


    internalTimer = new QTimer(this);
    connect(internalTimer, &QTimer::timeout, [=](){
        nextAlpha();
        prepareImage();
        update();
    });
}

ShadeLayer::~ShadeLayer()
{
    delete internalTimer;
}

void ShadeLayer::startShow()
{
    m_visible = true;
    m_pulse = false;
    m_alphaIncrement = ALPHA_INC_UP;
    if (!internalTimer->isActive()) {
        internalTimer->start(TIMER_TICKS);
    }
}

void ShadeLayer::startHide()
{
    m_visible = false;
    m_pulse = false;
    m_alphaIncrement = ALPHA_INC_DOWN;
    if (!internalTimer->isActive()) {
        internalTimer->start(TIMER_TICKS);
    }
}

void ShadeLayer::startPulse()
{
    m_pulse = true;
    m_visible = true;
    m_alphaIncrement = ALPHA_INC_UP;
    if (!internalTimer->isActive()) {
        internalTimer->start(TIMER_PULSE_TICKS);
    }
    emit pulseChanged(m_pulse);
}

void ShadeLayer::stopPulse()
{
    m_pulse = false;
    m_visible = false;
    m_alphaIncrement = ALPHA_INC_DOWN;
    if (!internalTimer->isActive()) {
        internalTimer->start(TIMER_PULSE_TICKS);
    }
    emit pulseChanged(m_pulse);
}

void ShadeLayer::nextAlpha()
{
    m_alpha += m_alphaIncrement;

    if (m_visible) {
        if (m_alpha > 1) {
            m_alpha = 1.0;
            if (m_pulse) {
                m_visible = false;
                m_alphaIncrement = ALPHA_INC_DOWN;
            } else {
                internalTimer->stop();
            }
        }
    } else {
        if (m_alpha < 0) {
            m_alpha = 0.0;
            if (m_pulse) {
                m_visible = true;
                m_alphaIncrement = ALPHA_INC_UP;
            } else {
                internalTimer->stop();
            }
        }
    }
}

void ShadeLayer::prepareImage()
{
    quint32 pix;
    int newAlpha;

    for (int x = 0; x < m_shadeImage.width(); ++x)
        for (int y = 0; y < m_shadeImage.height(); ++y) {
            pix = m_shadeImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = round (qreal(pix >> 24) * m_alpha);
                m_drawImage.setPixel(x, y, ((newAlpha & 0xff) << 24) | (pix & 0xffffff));
            }
        }

}

void ShadeLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, m_drawImage);
}

qreal ShadeLayer::scale()
{
    return CtGlobal::images().scale();
}

void ShadeLayer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

void ShadeLayer::onScaleChanged()
{
    setShade(m_shadeNumber);

    m_drawImage = QImage(CtGlobal::images().width(),
                         CtGlobal::images().height(),
                         QImage::Format_ARGB32);

    setWidth(CtGlobal::images().width());
    setHeight(CtGlobal::images().height());

    m_drawImage.fill(0x00ffffff);
    prepareImage();
    update();
    emit scaleChanged(scale());
}

int ShadeLayer::shade()
{
    return m_shadeNumber;
}

bool ShadeLayer::visible()
{
    return m_visible;
}

bool ShadeLayer::pulse()
{
    return m_pulse;
}

void ShadeLayer::setShade(int newShadeNumber)
{
    m_shadeNumber = newShadeNumber;
    switch (m_shadeNumber)
    {
    case 0:
        startHide();
        break;
    case 1:
        m_shadeImage = CtGlobal::images().shadeYellow().toImage();
        break;
    case 2:
        m_shadeImage = CtGlobal::images().shadeGreen().toImage();
        break;
    case 3:
        m_shadeImage = CtGlobal::images().shadeBlue().toImage();
        break;
    }
}


void ShadeLayer::setPulse(bool newPulse)
{
    if (m_pulse != newPulse) {
        if (newPulse)
            startPulse();
        else
            stopPulse();
    }
}

