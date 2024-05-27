#include "bottlelayer.h"

#include <QPixmap>
#include <QPainter>
#include <QtDebug>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/tubeimages.h"
#include "tubeitem.h"

BottleLayer::BottleLayer(TubeItem * parent) :
      QQuickPaintedItem((QQuickItem *) parent)
{
    m_tube = parent;

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

    if (qFuzzyIsNull(m_tube->angle())) {

        painter->drawPixmap(0, (m_startY + CtGlobal::images().shiftHeight()), m_drawPixMap);

    } else {

        qreal x = (m_tube->angle() > 0)
                  ? CtGlobal::images().vertex(0).x()
                  : CtGlobal::images().vertex(5).x();
        x += CtGlobal::images().shiftWidth();

        qreal y = CtGlobal::images().vertex(0).y()
                + CtGlobal::images().shiftHeight()
                - m_tube->m_verticalShift;

        painter->translate(x, y);
        painter->rotate(m_tube->angle() * CT_RAD2DEG);
        painter->translate(-x, -y);

        painter->drawPixmap(
                    CtGlobal::images().shiftWidth(),
                    m_startY + CtGlobal::images().shiftHeight() - m_tube->m_verticalShift,
                    m_drawPixMap);
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
            newSource.compare("whole", Qt::CaseInsensitive) == 0) {
        setSource(CT_BOTTLE_WHOLE);
    } else if (newSource.compare("front", Qt::CaseInsensitive) == 0) {
        setSource(CT_BOTTLE_FRONT);
    } else if (newSource.compare("back", Qt::CaseInsensitive) == 0) {
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
        m_drawPixMap = CtGlobal::images().bottle();
        m_startY = 0.0;
        break;
    case CT_BOTTLE_FRONT:
        m_source_id = newSourceId;
        m_drawPixMap = CtGlobal::images().bottleFront();
        m_startY = CtGlobal::images().vertex(0).y();
        break;
    case CT_BOTTLE_BACK:
        m_source_id = newSourceId;
        m_drawPixMap = CtGlobal::images().bottleBack();
        m_startY = 0.0;
        break;
    default:
        m_source_id = CT_BOTTLE_NONE;
        m_startY = 0.0;
    }

    if (m_source_id != CT_BOTTLE_NONE) {
        setWidth(CtGlobal::images().tubeFullWidth());
        setHeight(CtGlobal::images().tubeFullHeight());
        update();
    }
}

void BottleLayer::onScaleChanged()
{
    if (m_source_id != CT_BOTTLE_NONE) {
        setSource(m_source_id);
    }
}

void BottleLayer::onAngleChanged()
{
    if (m_source_id != CT_BOTTLE_NONE) {
        update();
    }
}


