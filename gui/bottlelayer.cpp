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

bool BottleLayer::visible()
{
    return m_visible;
}

void BottleLayer::setSource(QString newSource)
{
    if (newSource.compare("bottle", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().bottleFront();
    }
    else if (newSource.compare("back", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().bottleBack();
    }

    else if (newSource.compare("cork", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().cork();
    }

    setWidth(m_drawImage.width());
    setHeight(m_drawImage.height());
}

void BottleLayer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

void BottleLayer::onScaleChanged()
{
    setSource(m_source);
    update();
    emit scaleChanged(CtGlobal::images().scale());
}



