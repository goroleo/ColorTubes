#include "tubeflyer.h"

#include "src/ctglobal.h"
#include "src/tubeimages.h"


TubeFlyer::TubeFlyer(QQuickItem *parent) :
      QQuickPaintedItem(parent),
      m_angle(0.0)
{
    internalTimer = new QTimer(this);
    connect(internalTimer, &QTimer::timeout, [=](){
//        nextAlpha();
//        prepareImage();
        update();
    });
}


TubeFlyer::~TubeFlyer()
{
    delete internalTimer;
}


void TubeFlyer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, m_drawImage);
}

qreal TubeFlyer::scale()
{
    return CtGlobal::images().scale();
}


void TubeFlyer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

void TubeFlyer::onScaleChanged()
{
    m_drawImage = QImage(280 * scale(),
                         200 * scale(),
                         QImage::Format_ARGB32);
    m_drawImage.fill(0x00ffffff);

    update();
    emit scaleChanged(scale());
}

qreal TubeFlyer::angle()
{
    return m_angle;
}

void TubeFlyer::setAngle(qreal newAngle)
{
    m_angle = newAngle;
    calculatePoints();
}

void TubeFlyer::calculatePoints()
{
}



