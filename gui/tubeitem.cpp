#include "tubeitem.h"

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

//    setScale(1.145);
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

    onScaleChanged();
    setAngle(115.11 / 180.0 * M_PI);

}

TubeItem::~TubeItem()
{
    delete cork;
    delete front;
    delete colors;
    delete back;
    delete shade;
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
    shade->setX(100 * scale());
    shade->setY(20 * scale());
    cork->setX(shade->x());
}


qreal TubeItem::angle() const
{
    return m_angle;
}

void TubeItem::setAngle(qreal newAngle)
{
    if (qFuzzyCompare(m_angle, newAngle))
        return;

    m_angle = newAngle;
    emit angleChanged(newAngle);
}

