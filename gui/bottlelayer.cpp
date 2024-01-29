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
    if (qFuzzyIsNull(m_angle)) {

        setWidth(m_drawImage.width());
        setHeight(m_drawImage.height());
        painter->drawPixmap(0, 0, m_drawImage);

    } else {

        qreal cos = qCos(m_angle);
        qreal sin = qSin(m_angle);
        setWidth(280 * scale());
        setHeight(200 * scale());

        qreal x = (m_angle > 0)
                  ? CtGlobal::images().vertex(0).x()
                  : CtGlobal::images().vertex(5).x();

        qreal y = CtGlobal::images().vertex(0).y();

//        painter->translate(-x, -y);
        painter->rotate(m_angle / M_PI * 180);
//        painter->translate(x, y);
        painter->drawPixmap(x, y, m_drawImage);

        qDebug() << x << y << width() << height();

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
        update();
        qDebug() << "bottleLayer::setSource" << m_source << m_drawImage.width() << m_drawImage.height();
    }
}

void BottleLayer::onScaleChanged()
{
    if (findImage(m_source)) {
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


