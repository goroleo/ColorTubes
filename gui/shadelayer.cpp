#include "shadelayer.h"

#include <QPixmap>
#include <QtDebug>

#include "src/ctglobal.h"
#include "src/ctimages.h"

ShadeLayer::ShadeLayer(QQuickItem *parent) :
      QQuickPaintedItem(parent),
      m_shadeNumber(0)
{
    m_drawImage = QImage(CtGlobal::images().tubeWidth(),
                         CtGlobal::images().tubeHeight(),
                         QImage::Format_ARGB32);

    setWidth(CtGlobal::images().tubeWidth());
    setHeight(CtGlobal::images().tubeHeight());

    m_drawImage.fill(0x00ffffff);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, [=](){
        nextFrame();
        paintFrame();
        update();
    });
}

ShadeLayer::~ShadeLayer()
{
    delete m_timer;
}

void ShadeLayer::startShow()
{
    if (m_animated) {
        m_visible = true;
        m_pulse = false;
        m_alphaIncrement = 1.0 / qreal(CT_SHADE_STEPS_INC);
        m_timer->start(CT_TIMER_TICKS);
    } else
        showImmediately();
}

void ShadeLayer::startHide()
{
    if (m_animated) {
        m_visible = false;
        m_pulse = false;
        m_alphaIncrement = -1.0 / qreal(CT_SHADE_STEPS_DEC);
        m_timer->start(CT_TIMER_TICKS);
    } else
        hideImmediately();
}

void ShadeLayer::showImmediately()
{
    m_visible = true;
    m_pulse = false;
    m_alpha = 1.0;
    paintFrame();
    update();
}

void ShadeLayer::hideImmediately()
{
    m_visible = false;
    m_pulse = false;
    m_alpha = 0.0;
    paintFrame();
    update();
    if (m_shadeAfterHiding > 0) {
        setShade(m_shadeAfterHiding);
        startShow();
        m_shadeAfterHiding = 0;
    }
}

void ShadeLayer::startPulse()
{
    if (m_animated) {
        m_pulse = true;
        m_visible = true;
        m_alphaIncrement = 1.0 / qreal(CT_SHADE_STEPS_INC);
        m_timer->start(CT_TIMER_TICKS * 3);
        emit pulseChanged(m_pulse);
    }
}

void ShadeLayer::stopPulse()
{
    m_pulse = false;
    m_visible = false;
    m_alphaIncrement = -1.0 / qreal(CT_SHADE_STEPS_DEC);
    m_timer->start(CT_TIMER_TICKS);
    emit pulseChanged(m_pulse);
}

void ShadeLayer::nextFrame()
{
    m_alpha += m_alphaIncrement;

    if (m_visible) {
        if (m_alpha > 1) {
            m_alpha = 1.0;
            if (m_pulse) {
                m_visible = false;
                m_alphaIncrement = -1.0 / qreal(CT_SHADE_STEPS_DEC);
            } else {
                m_timer->stop();
            }
        }
    } else {
        if (m_alpha < 0) {
            m_alpha = 0.0;
            if (m_pulse) {
                m_visible = true;
                m_alphaIncrement = 1.0 / qreal(CT_SHADE_STEPS_INC);
            } else {
                m_timer->stop();

                if (m_shadeAfterHiding > 0) {
                    setShade(m_shadeAfterHiding);
                    startShow();
                    m_shadeAfterHiding = 0;
                }
            }
        }
    }
}

void ShadeLayer::paintFrame()
{
    quint32 pix;
    int newAlpha;

    for (int x = 0; x < m_shadeImage.width(); ++x) {
        for (int y = 0; y < m_shadeImage.height(); ++y) {
            pix = m_shadeImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = qRound (qreal(pix >> 24) * m_alpha);
                m_drawImage.setPixel(x, y, ((newAlpha & 0xff) << 24) | (pix & 0xffffff));
            }
        }
    }
}

void ShadeLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, m_drawImage);
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
    paintFrame();
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
//    if (m_shadeNumber == newShadeNumber)
//        return;
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
}

void ShadeLayer::setShadeAfterHide(int newShadeNumber)
{
    if (m_timer->isActive() || isVisible()) {
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

void ShadeLayer::setAnimated(bool value)
{
    m_animated = value;
    if (!m_animated && m_timer->isActive()) {
        m_timer->stop();
        if (m_visible)
            showImmediately();
        else
            hideImmediately();
    }
}

