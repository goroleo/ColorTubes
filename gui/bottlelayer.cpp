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

void BottleLayer::paint(QPainter *painter)
{

    if (qFuzzyIsNull(m_angle)) {

        painter->drawPixmap(0, dy * scale(), m_drawImage);

    } else {

        qreal x = (m_angle > 0)
                  ? CtGlobal::images().vertex(0).x()
                  : CtGlobal::images().vertex(5).x();
        x += 100 * scale();

        qreal y = CtGlobal::images().vertex(0).y() + 20 * scale();

        painter->translate(x, y);
        painter->rotate(m_angle * CT_RAD2DEG);
        painter->translate(-x, -y);

        painter->drawPixmap(100 * scale(),
                            dy * scale(),
                            m_drawImage);
    }
}

quint8 BottleLayer::sourceId()
{
    return m_source_id;
}

QString BottleLayer::source()
{
    switch (m_source_id) {
    case CT_BOTTLE_WHOLE:
        return QString("bottle");
    case CT_BOTTLE_FRONT:
        return QString("front");
    case CT_BOTTLE_BACK:
        return QString("back");
    default:
        return QString("");
    }
}

void BottleLayer::setSource(QString newSource)
{
    if (newSource.compare("bottle", Qt::CaseInsensitive) == 0 ||
            newSource.compare("whole", Qt::CaseInsensitive) == 0)
    {
        setSource(CT_BOTTLE_WHOLE);
    }
    else if (newSource.compare("front", Qt::CaseInsensitive) == 0) {
        setSource(CT_BOTTLE_FRONT);
    }
    else if (newSource.compare("back", Qt::CaseInsensitive) == 0) {
        setSource(CT_BOTTLE_BACK);
    } else {
        setSource(CT_BOTTLE_NONE);
    }
}

void BottleLayer::setSource(quint8 newSourceId)
{
    switch (newSourceId) {
    case CT_BOTTLE_WHOLE:
        m_source_id = newSourceId;
        m_drawImage = CtGlobal::images().bottle();
        dy = 20.0;
        break;
    case CT_BOTTLE_FRONT:
        m_source_id = newSourceId;
        m_drawImage = CtGlobal::images().bottleFront();
        dy = 35.5;
        break;
    case CT_BOTTLE_BACK:
        m_source_id = newSourceId;
        m_drawImage = CtGlobal::images().bottleBack();
        dy = 20.0;
        break;
    default:
        m_source_id = CT_BOTTLE_NONE;
        dy = 0.0;
    }

    if (m_source_id != CT_BOTTLE_NONE) {
        setWidth(280 * scale());
        setHeight(200 * scale());
        update();
    }
}

qreal BottleLayer::scale()
{
    return CtGlobal::images().scale();
}

void BottleLayer::onScaleChanged()
{
    if (m_source_id != CT_BOTTLE_NONE) {
        setSource(m_source_id);
        update();
    }
}

void BottleLayer::onAngleChanged()
{
    if (qFuzzyCompare(m_angle, parentTube->angle()))
            return;

    if (m_source_id != CT_BOTTLE_NONE) {
        m_angle = parentTube->angle();
        update();
    }
}


