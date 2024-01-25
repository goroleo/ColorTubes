#include "bottlelayer.h"

#include <QPixmap>
#include <QPainter>
#include <QtDebug>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

BottleLayer::BottleLayer(TubeItem *parent) :
      QQuickPaintedItem((QQuickItem*) parent)
{
    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

//    m_drawImage = nullptr;
//    onScaleChanged();
}

BottleLayer::~BottleLayer()
{
}

void BottleLayer::prepareImage()
{
}

void BottleLayer::paint(QPainter *painter)
{
//    painter->rotate(m_angle);
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

bool BottleLayer::findImage(QString aSource)
{
    bool result = false;
    if (aSource.compare("bottle", Qt::CaseInsensitive) == 0) {
        m_drawImage = CtGlobal::images().bottle();
        result = true;
    }
    else if (aSource.compare("front", Qt::CaseInsensitive) == 0) {
        m_drawImage = CtGlobal::images().bottleFront();
        result = true;
    }
    else if (aSource.compare("back", Qt::CaseInsensitive) == 0) {
        m_drawImage = CtGlobal::images().bottleBack();
        result = true;
    }
    return result;
}


void BottleLayer::setSource(QString newSource)
{
    if (findImage(newSource)) {
        m_source = newSource;
        setWidth(m_drawImage.width());
        setHeight(m_drawImage.height());
        update();

        qDebug() << "bottleLayer::setSource" << m_source << m_drawImage.width() << m_drawImage.height();
    }

}

void BottleLayer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

void BottleLayer::setAngle(qreal newAngle)
{
    m_angle = newAngle;
//    setWidth(qMax(m_drawImage.width(), m_drawImage.height()));
//    setHeight(width());
}

void BottleLayer::onScaleChanged()
{

    if (findImage(m_source)) {
        setWidth(m_drawImage.width());
        setHeight(m_drawImage.height());
        update();

        qDebug() << "bottleLayer::onScaleChanged" << m_source << m_drawImage.width() << m_drawImage.height();
    }
    //    emit scaleChanged(CtGlobal::images().scale());
}



