#include "bottlelayer.h"

#include <QPixmap>
#include <QPainter>
#include <QtDebug>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

BottleLayer::BottleLayer(QQuickItem *parent) :
      QQuickPaintedItem(parent)
{
    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));
}

BottleLayer::~BottleLayer()
{
}

void BottleLayer::prepareImage()
{
}

void BottleLayer::paint(QPainter *painter)
{
    painter->rotate(m_angle);
    painter->drawPixmap(0, 0, m_drawImage);
}

QString BottleLayer::source()
{
    return m_source;
}

qreal BottleLayer::scale()
{
    return CtGlobal::images().scale();
}

void BottleLayer::setSource(QString newSource)
{
    if (newSource.compare("bottle", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().bottle();
    }
    else if (newSource.compare("front", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().bottleFront();
    }
    else if (newSource.compare("back", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().bottleBack();
    }

}

void BottleLayer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
    update();
}

void BottleLayer::setAngle(qreal newAngle)
{
    m_angle = newAngle;
    setWidth(qMax(m_drawImage.width(), m_drawImage.height()));
    setHeight(width());
}

void BottleLayer::onScaleChanged()
{
    setSource(m_source);
    update();
    emit scaleChanged(CtGlobal::images().scale());
}



