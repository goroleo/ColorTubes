#include "shadelayer.h"

#include <QPixmap>
#include <QtDebug>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

ShadeLayer::ShadeLayer(QQuickItem *parent) :
      QQuickPaintedItem(parent),
      m_shadeNumber(1)
{
    m_drawImage = QImage(CtGlobal::images().tubeWidth(),
                         CtGlobal::images().tubeHeight(),
                         QImage::Format_ARGB32);

    setWidth(CtGlobal::images().tubeWidth());
    setHeight(CtGlobal::images().tubeHeight());

    m_drawImage.fill(0x00ffffff);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    shadingTimer = new QTimer(this);

    connect(shadingTimer, &QTimer::timeout, [=](){
        nextAlpha();
        prepareImage();
        update();
    });
}

ShadeLayer::~ShadeLayer()
{
    delete shadingTimer;
}

void ShadeLayer::startShow()
{
    m_visible = true;
    m_pulse = false;
    m_alphaIncrement = ALPHA_INC_UP;
    shadingTimer->start(TIMER_TICKS);
}

void ShadeLayer::startHide()
{
    m_visible = false;
    m_pulse = false;
    m_alphaIncrement = ALPHA_INC_DOWN;
    m_shadeAfterHiding = 0;
    shadingTimer->start(TIMER_TICKS);
}

void ShadeLayer::hideImmediately()
{
    m_visible = false;
    m_pulse = false;
    m_alpha = 0.0;
    prepareImage();
    update();
    if (m_shadeAfterHiding > 0) {
        setShade(m_shadeAfterHiding);
        startShow();
        m_shadeAfterHiding = 0;
    }
}

void ShadeLayer::startPulse()
{
    m_pulse = true;
    m_visible = true;
    m_alphaIncrement = ALPHA_INC_UP;
    shadingTimer->start(TIMER_PULSE_TICKS);
    emit pulseChanged(m_pulse);
}

void ShadeLayer::stopPulse()
{
    m_pulse = false;
    m_visible = false;
    m_alphaIncrement = ALPHA_INC_DOWN;
    shadingTimer->start(TIMER_PULSE_TICKS);
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
                shadingTimer->stop();
            }
        }
    } else {
        if (m_alpha < 0) {
            m_alpha = 0.0;
            if (m_pulse) {
                m_visible = true;
                m_alphaIncrement = ALPHA_INC_UP;
            } else {
                shadingTimer->stop();

                if (m_shadeAfterHiding > 0) {
                    setShade(m_shadeAfterHiding);
                    startShow();
                    m_shadeAfterHiding = 0;
                }
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
                newAlpha = qRound (qreal(pix >> 24) * m_alpha);
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

void ShadeLayer::onScaleChanged()
{
    setShade(m_shadeNumber);

    m_drawImage = QImage(CtGlobal::images().tubeWidth(),
                         CtGlobal::images().tubeHeight(),
                         QImage::Format_ARGB32);

    setWidth(CtGlobal::images().tubeWidth());
    setHeight(CtGlobal::images().tubeHeight());

    m_drawImage.fill(0x00ffffff);
    prepareImage();
    update();
}

int ShadeLayer::shade()
{
    if (m_visible)
        return m_shadeNumber;
    else
        return 0;
}

bool ShadeLayer::isVisible()
{
    return m_visible;
}

bool ShadeLayer::pulse()
{
    return m_pulse;
}

void ShadeLayer::setShade(int newShadeNumber)
{
    if (m_shadeNumber == newShadeNumber)
        return;
    m_shadeNumber = newShadeNumber;
    switch (m_shadeNumber)
    {
    case 1:
        m_shadeImage = CtGlobal::images().shadeYellow().toImage();
        break;
    case 2:
        m_shadeImage = CtGlobal::images().shadeGreen().toImage();
        break;
    case 3:
        m_shadeImage = CtGlobal::images().shadeBlue().toImage();
        break;
    case 4:
        m_shadeImage = CtGlobal::images().shadeRed().toImage();
        break;
    case 5:
        m_shadeImage = CtGlobal::images().shadeGray().toImage();
        break;
    default:
        m_shadeNumber = 0;
        startHide();
        break;
    }
    emit shadeChanged(m_shadeNumber);
}

void ShadeLayer::setShadeAfterHiding(int newShadeNumber)
{
    if (shadingTimer->isActive() || isVisible()) {
        m_shadeAfterHiding = newShadeNumber;
        startHide();
    } else {
        setShade(newShadeNumber);
        startShow();
    }
}

void ShadeLayer::setPulse(bool value)
{
    if (m_pulse != value) {
        if (value)
            startPulse();
        else
            stopPulse();
        emit pulseChanged(value);
    }
}

