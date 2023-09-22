#include "tubeitem.h"

#include "src/ctglobal.h"
#include "src/tubeimages.h"

#include "core/tubemodel.h"

#include "bottlelayer.h"
#include "corklayer.h"
#include "colorslayer.h"
#include "shadelayer.h"

TubeItem::TubeItem(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    model = new TubeModel();
    model->putColor(12);
    model->putColor(8);
    model->putColor(1);
    model->putColor(10);


    setScale(2);
    shade = new ShadeLayer(this);
    shade->setX(100*scale());
    shade->setY(20*scale());
    shade->setShade(3);
    shade->startShow();

    back = new BottleLayer(this);
    back->setSource("back");
    back->setX(100*scale());
    back->setY(20*scale());

    colors = new ColorsLayer(this);
    colors->setModel(model);
    colors->setAngle(-30.0/180.0*M_PI);
//    colors->setY(20*scale());

    front = new BottleLayer(this);
    front->setSource("front");
    front->setX(100*scale());
    front->setY(35.5*scale());

    cork = new CorkLayer(this);
    cork->setVisible(true);
    cork->setX(100*scale());
    cork->setY(0);


    setClip(true);
}

TubeItem::~TubeItem()
{
    delete cork;
    delete front;
    delete colors;
    delete back;
    delete shade;
}

void TubeItem::paint(QPainter *painter)
{
//    setWidth(clipRect().width());
//    setHeight(clipRect().height());
    setX(-clipRect().x());
    qDebug() << "x" << x();
}

qreal TubeItem::scale() const
{
    return CtGlobal::images().scale();
}

void TubeItem::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

QRectF TubeItem::clipRect() const
{
    qDebug() << "clipRect";
    return QRectF(100.0 * scale(), 0, 80.0 * scale(), 200.0 * scale());
}


