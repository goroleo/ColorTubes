#include "bottlelayer.h"

#include <QPixmap>
#include <QPainter>
#include <QtDebug>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/tubeimages.h"
#include "tubeitem.h"

BottleLayer::BottleLayer(TubeItem *parent) :
      QQuickPaintedItem((QQuickItem *) parent)
{

    parentTube = parent;

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    QObject::connect(parent, &TubeItem::angleChanged,
            this, &BottleLayer::onAngleChanged);

    setAntialiasing(true);
}

BottleLayer::~BottleLayer()
{
}

void BottleLayer::prepareImage()
{
}

void BottleLayer::paint(QPainter *painter)
{
//    painter->setBrush(QColor(0,127,0,80));
//    painter->drawRect(0,0,width(), height());


    if (qFuzzyIsNull(m_angle)) {

        painter->drawPixmap(100 * scale(), dy * scale(), m_drawImage);

    } else {

        qreal x = (m_angle > 0)
                  ? CtGlobal::images().vertex(0).x()
                  : CtGlobal::images().vertex(5).x();
        x = x + 100 * scale();

        qreal y = (m_source.compare("back", Qt::CaseInsensitive) == 0)
                ? m_drawImage.height() + dy * scale()
                : dy * scale();

        painter->translate(-x, -y);
        painter->rotate(m_angle / M_PI * 180);
        painter->translate(x, y);
        painter->drawPixmap(0, 0, m_drawImage);

        qDebug() << "Vertex:" << x << y;
        qDebug() << x << y << dy * scale();

/*
        qreal cos = qCos(m_angle);
        qreal sin = qSin(m_angle);
        setWidth(m_drawImage.width() * cos);
        setHeight(m_drawImage.height() * sin);

        QTransform trans = QTransform::setMatrix(
                    cos, -sin, (x * cos - y * sin - x),
                    sin, cos, (x * sin + y * cos - y),
                    0, 0, 1);

        painter->setTransform();
*/


    }

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
        dy = 20.0;
        result = true;
    }
    else if (aSource.compare("front", Qt::CaseInsensitive) == 0) {
        m_drawImage = CtGlobal::images().bottleFront();
        dy = 35.5;
        result = true;
    }
    else if (aSource.compare("back", Qt::CaseInsensitive) == 0) {
        m_drawImage = CtGlobal::images().bottleBack();
        dy = 20.0;
        result = true;
    }
    return result;
}


void BottleLayer::setSource(QString newSource)
{
    if (findImage(newSource)) {
        m_source = newSource;
        setWidth(280 * scale());
        setHeight(200 * scale());
        update();
        qDebug() << "bottleLayer::setSource" << m_source << m_drawImage.width() << m_drawImage.height();
    }
}

void BottleLayer::onScaleChanged()
{
    if (findImage(m_source)) {
        setWidth(280 * scale());
        setHeight(200 * scale());
        update();
        qDebug() << "bottleLayer::onScaleChanged" << m_source << m_drawImage.width() << m_drawImage.height();
    }
}

void BottleLayer::onAngleChanged()
{
    if (qFuzzyCompare(m_angle, parentTube->angle()))
            return;

    if (findImage(m_source)) {
        m_angle = parentTube->angle();
        update();
        qDebug() << "bottleLayer::onAngleChanged" << m_source << m_angle << m_angle / M_PI * 180;
    }
}


