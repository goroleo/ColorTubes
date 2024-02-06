#include "tubeitem.h"

#include <QSGClipNode>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

#include "core/tubemodel.h"

#include "bottlelayer.h"
#include "corklayer.h"
#include "colorslayer.h"
#include "shadelayer.h"

TubeItem::TubeItem(QQuickItem *parent) :
    QQuickItem(parent)
{
    model = new TubeModel();
    model->putColor(12);
    model->putColor(8);
    model->putColor(1);
    model->putColor(10);

    shade = new ShadeLayer(this);
    shade->setVisible(true);
    shade->setShade(0);
    shade->startShow();

    back = new BottleLayer(this);
    back->setSource("back");
    back->setVisible(true);

    colors = new ColorsLayer(this);
    colors->setModel(model);

    front = new BottleLayer(this);
    front->setSource("front");

    cork = new CorkLayer(this);
    cork->setVisible(false);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

//    onScaleChanged();
//    setAngle(15.11 / 180.0 * M_PI);

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

}

TubeItem::~TubeItem()
{
    delete cork;
    delete front;
    delete colors;
    delete back;
    delete shade;
}

void TubeItem::mousePressEvent(QMouseEvent* event)
{
    QQuickItem::mousePressEvent(event);
    setAngle(m_angle - CT_2PI * 4 + (180.0 + 90.0)/180.0 * CT_PI);
    qDebug() << event->pos();
}

qreal TubeItem::scale() const
{
    return CtGlobal::images().scale();
}

void TubeItem::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

void TubeItem::onScaleChanged()
{
    resize();
}

qreal TubeItem::angle() const
{
    return m_angle;
}

void TubeItem::setAngle(qreal newAngle)
{

    newAngle = fmod(newAngle, CT_2PI);
    if (qAbs(newAngle) > CT_PI) {
        newAngle = std::signbit(newAngle)
                ? CT_2PI + newAngle
                : newAngle - CT_2PI;
    }

    if (qFuzzyCompare(m_angle, newAngle))
        return;

    m_angle = newAngle;
    resize();

    emit angleChanged(newAngle);
}


void TubeItem::resize()
{
    shade->setY(20 * scale());
    back->setX(0);
    colors->setX(0);
    front->setX(0);

    if (qFuzzyIsNull(m_angle)) {

        shade->setX(0);
        colors->setY(shade->y());
        cork->setX(0);

        setWidth(80 * scale());
        setHeight(200 * scale());
        setClip(true);

    } else {

        shade->setX(100 * scale());
        colors->setY(0);
        cork->setX(shade->x());

        setWidth(0);
        setHeight(0);
        setClip(false);
    }
}

