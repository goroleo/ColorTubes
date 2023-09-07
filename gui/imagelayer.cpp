#include "imagelayer.h"

#include <QPixmap>
#include <QtDebug>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

CtImageLayer::CtImageLayer(QQuickItem *parent) :
      QQuickPaintedItem(parent)
{
}

CtImageLayer::~CtImageLayer()
{
}

void CtImageLayer::prepareImage()
{
}

void CtImageLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, m_drawImage);
}

QString CtImageLayer::source()
{
    return m_source;
}

qreal CtImageLayer::scale()
{
    return CtGlobal::images().scale();
}

bool CtImageLayer::visible()
{
    return m_visible;
}

void CtImageLayer::setSource(QString newSource)
{
    if (newSource.compare("bottle", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().bottle().toImage();
    }
    else if (newSource.compare("cork", Qt::CaseInsensitive) == 0)
    {
        m_source = newSource;
        m_drawImage = CtGlobal::images().cork().toImage();
    }

    setWidth(m_drawImage.width());
    setHeight(m_drawImage.height());
}

void CtImageLayer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
    setSource(m_source);
    update();
    emit scaleChanged(CtGlobal::images().scale());
}



