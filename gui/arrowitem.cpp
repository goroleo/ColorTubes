#include "arrowitem.h"

#include "src/ctglobal.h"
#include "src/ctimages.h"

ArrowItem::ArrowItem(QQuickItem * parent) :
    QQuickPaintedItem(parent)
{
    m_bodyTimer = new QTimer(this);
    m_strokeTimer = new QTimer(this);

    setAntialiasing(true);
    setClip(true);
    onScaleChanged();

    QObject::connect(&CtGlobal::images(), &CtImages::scaleChanged,
                     this, &ArrowItem::onScaleChanged);

    connect(m_bodyTimer, &QTimer::timeout, [=](){
        if (m_arrowId != CT_ARROW_NONE) {
            nextBodyFrame();
            paintBody();
            update();
        }
    });

    connect(m_strokeTimer, &QTimer::timeout, [=](){
        if (m_arrowId != CT_ARROW_NONE) {
            nextStrokeFrame();
            paintStroke();
            update();
        }
    });
}

ArrowItem::~ArrowItem()
{
    delete m_bodyTimer;
    delete m_strokeTimer;
}

void ArrowItem::setArrowId(quint8 arrowId)
{
    if (arrowId == CT_ARROW_IN
            || arrowId == CT_ARROW_OUT)
        m_arrowId = arrowId;
    else
        m_arrowId = CT_ARROW_NONE;
    onScaleChanged();
}

void ArrowItem::paint(QPainter *painter)
{
    if (m_arrowId != CT_ARROW_NONE) {
        painter->drawImage(0, 0, m_drawBodyImage);
        painter->drawImage(0, 0, m_drawStrokeImage);
    }
}

void ArrowItem::onScaleChanged()
{
    switch (m_arrowId) {
    case CT_ARROW_IN:
        m_bodyImage = CtGlobal::images().arrowInBody().toImage();
        m_strokeImage = CtGlobal::images().arrowInStroke().toImage();
        break;
    case CT_ARROW_OUT:
        m_bodyImage = CtGlobal::images().arrowOutBody().toImage();
        m_strokeImage = CtGlobal::images().arrowOutStroke().toImage();
        break;
    default:
        m_arrowId = CT_ARROW_NONE;
        setWidth(0);
        setHeight(0);
        break;
    }

    if (m_arrowId != CT_ARROW_NONE) {
        setWidth(m_bodyImage.width());
        setHeight(m_bodyImage.height());
        m_drawBodyImage = QImage(m_bodyImage.width(),
                             m_bodyImage.height(),
                             QImage::Format_ARGB32);
        m_drawStrokeImage = QImage(m_strokeImage.width(),
                             m_strokeImage.height(),
                             QImage::Format_ARGB32);
        m_bodyOpacity = 0.0;
        m_strokeOpacity = 0.0;
        paintStroke();
        paintBody();
        update();
    }
}

void ArrowItem::setVisible(bool value)
{
    if (m_visible == value)
        return;

    if (value) {
        startShow();
    } else
        startHide();
}

void ArrowItem::startShow()
{
    if (m_arrowId == CT_ARROW_NONE)
        return;
    m_strokeOpacity = 0.0;
    m_bodyOpacity = 0.0;
    m_visible = true;
    m_blinken = false;
    m_strokeTimer->stop();
    m_opacityIncrement = 1.0 / CT_ARROW_STEPS_INC;
    m_bodyOpacity = - m_opacityIncrement;
    m_bodyTimer->start(CT_TIMER_TICKS);
}

void ArrowItem::startHide()
{
    m_visible = false;
    m_blinken = false;
    m_strokeTimer->stop();
    if (m_arrowId == CT_ARROW_NONE)
        return;
    m_opacityIncrement = - 1.0 / CT_ARROW_STEPS_DEC;
    m_bodyTimer->start(CT_TIMER_TICKS);
}

void ArrowItem::hideImmediately()
{
    m_visible = false;
    m_blinken = false;
    m_strokeTimer->stop();
    m_bodyTimer->stop();
    m_bodyOpacity = 0.0;
    m_strokeOpacity = 0.0;
    paintBody();
    paintStroke();
    update();
}

void ArrowItem::startBlink()
{
    m_blinken = true;
    m_visible = true;
    m_strokeOpacity = 0.0;
    m_opacityIncrement = 1.0 / qreal(CT_SHADE_STEPS_INC);
    m_strokeTimer->start(CT_BLINK_TICKS);
}

void ArrowItem::stopBlink()
{
    m_blinken = false;
    m_visible = true;
    m_opacityIncrement = - 1.0 / qreal(CT_SHADE_STEPS_DEC);
}

void ArrowItem::nextBodyFrame()
{
    m_bodyOpacity += m_opacityIncrement;
    if (m_visible) {
        if (m_bodyOpacity > 1.0) {
            m_bodyOpacity = 1.0;
            m_bodyTimer->stop();
            emit shown();
        }
    } else {
        if (m_bodyOpacity < 0.0) {
            m_bodyOpacity = 0.0;
            m_strokeOpacity = 0.0;
            m_bodyTimer->stop();
        }
        m_strokeOpacity += m_opacityIncrement;
        if (m_strokeOpacity < 0.0)
                m_strokeOpacity = 0.0;
        paintStroke();
    }
}

void ArrowItem::nextStrokeFrame()
{
    m_strokeOpacity += m_opacityIncrement;
    if (m_strokeOpacity > 1.0) {
        m_strokeOpacity = 1.0;
        m_opacityIncrement = - 1.0 / qreal(CT_SHADE_STEPS_DEC);
    } else if (m_strokeOpacity < 0.0) {
        m_strokeOpacity = 0.0;
        if (m_blinken)
            m_opacityIncrement = 1.0 / qreal(CT_SHADE_STEPS_INC);
        else
            m_strokeTimer->stop();
    }
}

void ArrowItem::paintBody()
{
    m_drawBodyImage.fill(0x00ffffff);

    qreal currentOpacity;
    int viewportHeight = (m_visible)
            ? qRound(m_bodyOpacity * height())
            : height();
    int opacityHeight = viewportHeight / 2;
    int sourceY, destY;

    for (int y = 0; y < viewportHeight; ++y) {
        currentOpacity = 1.0;

        switch (m_arrowId) {
        case CT_ARROW_OUT:
            sourceY = y;
            destY = height() - viewportHeight + y;
            if (y > opacityHeight)
                currentOpacity = 2.0 - qreal(y) / qreal(opacityHeight);
            break;
        case CT_ARROW_IN:
            sourceY = height() - viewportHeight + y;
            destY = y;
            if (y < opacityHeight)
                currentOpacity = qreal(y) / qreal(opacityHeight);
            break;
        default:
            sourceY = destY = y;
        }

        if (!m_visible)
            currentOpacity *= m_bodyOpacity;

        for (int x = 0; x < m_bodyImage.width(); ++x) {
            quint32 pix = m_bodyImage.pixel(x, sourceY);
            int alpha = (pix >> 24) & 0xff;
            if (alpha > 0) {
                alpha = qRound( currentOpacity * alpha) & 0xff;
                pix = (pix & 0xffffff) + (alpha << 24);
                m_drawBodyImage.setPixel(x, destY, pix);
            }
        }
    }
}

void ArrowItem::paintStroke()
{
    m_drawStrokeImage.fill(0x00ffffff);
    if (m_strokeOpacity > 0.0) {
        for (int x = 0; x < m_strokeImage.width(); ++x) {
            for (int y = 0; y < m_strokeImage.height(); ++y) {
                quint32 pix = m_strokeImage.pixel(x, y);
                int alpha = (pix >> 24) & 0xff;
                if (alpha > 0) {
                    alpha = qRound(m_strokeOpacity * alpha) & 0xff;
                    pix = (pix & 0xffffff) + (alpha << 24);
                    m_drawStrokeImage.setPixel(x, y, pix);
                }
            }
        }
    }
}

void ArrowItem::setConnectedTube(TubeItem * tube)
{
    m_connectedTube = tube;
}

