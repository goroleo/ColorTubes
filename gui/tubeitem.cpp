#include "tubeitem.h"

#include <QSGClipNode>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

#include "core/tubemodel.h"

#include "bottlelayer.h"
#include "corklayer.h"
#include "colorslayer.h"
#include "shadelayer.h"

#include "gameboard.h"

TubeItem::TubeItem(QQuickItem *parent, TubeModel * tm) :
    QQuickItem(parent),
    m_model(tm)
{

    m_board = (GameBoard *) parent;

    m_shade = new ShadeLayer(this);
    m_shade->setShade(0);

    back = new BottleLayer(this);
    back->setSource(CT_BOTTLE_BACK);

    colors = new ColorsLayer(this, tm);

    front = new BottleLayer(this);
    front->setSource(CT_BOTTLE_FRONT);

    cork = new CorkLayer(this);
    cork->setVisible(false);

    placeLayers();

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    m_rotateTimer = new QTimer(this);
    connect(m_rotateTimer, &QTimer::timeout, [=]() {
        addAngleIncrement();
        update();
    });
}

TubeItem::~TubeItem()
{
    qDebug() << "TubeItem deleeted";

    delete m_rotateTimer;
    delete cork;
    delete front;
    delete colors;
    delete back;
    delete m_shade;
}

void TubeItem::mousePressEvent(QMouseEvent* event)
{
    QQuickItem::mousePressEvent(event);
    rotate();
}

int TubeItem::shade()
{
    return m_shade->shade();
}

void TubeItem::setShade(int newShade)
{
    m_shade->setShade(newShade);
    m_shade->startShow();
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
    placeLayers();
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

    if (qFuzzyIsNull(m_angle)) {
        if (m_discharged)
            placeLayers();
    } else {
        if (!m_discharged)
            placeLayers();
    }


    emit angleChanged(newAngle);
}

QPointF TubeItem::pivotPoint()
{
    if (qFuzzyIsNull(m_angle))
        return m_pivotPoint;

    qreal dx = 40 * scale();
    qreal dy = 20 * scale();

    if (m_angle > 0)
        return QPointF(m_pivotPoint.x() - dx, m_pivotPoint.x() - dy);

    return QPointF(m_pivotPoint.x() + dx, m_pivotPoint.x() - dy);
}

void TubeItem::setPivotPoint(QPointF newPoint)
{
    m_pivotPoint = newPoint;
    placeLayers();
}

void TubeItem::setYPrecision(qreal yp)
{
    m_yPrecision = yp;
    setY(m_pivotPoint.y() + m_yPrecision);
}

void TubeItem::placeLayers()
{
    setY(m_pivotPoint.y());

    if (qFuzzyIsNull(m_angle)) {

            m_discharged = false;
            m_shade->setY(20 * scale());

            colors->setY(m_shade->y());

            setX(m_pivotPoint.x());
            setY(m_pivotPoint.y());

            setWidth(80 * scale());
            setHeight(200 * scale());
            setClip(true);

    } else {

            m_discharged = true;
            colors->setY(0);
            setX(m_pivotPoint.x() - 100 * scale());
            setY(m_pivotPoint.y());

            setWidth(0);
            setHeight(0);
            setClip(false);
    }
}

void TubeItem::rotate()
{
    m_angleIncrement = std::copysign(1.5 * CT_DEG2RAD, m_angleIncrement);
    setZ(m_board->maxChildrenZ() + 1); // above all other tubes

    qreal dx = std::copysign(40*scale(), m_angleIncrement);

    setPivotPoint(QPointF(m_pivotPoint.x()-dx, m_pivotPoint.y() - 20 * scale()));
    m_rotateTimer->start(1);
}

void TubeItem::addAngleIncrement()
{
    setAngle(m_angle + m_angleIncrement);
    if (abs(m_angle) >= 118.6105 * CT_DEG2RAD)
        m_angleIncrement = - m_angle / 30.0;
    if (qFuzzyIsNull(m_angle)) {
        setAngle(0);

        qreal dx = std::copysign(40*scale(), m_angleIncrement);

        setPivotPoint(QPointF(m_pivotPoint.x()-dx, m_pivotPoint.y() + 20*scale()));
        m_rotateTimer->stop();
        setZ(0);
    }
}

bool TubeItem::isCLosed()
{
    return m_model->isClosed();
}

bool TubeItem::isEmpty()
{
    return m_model->isEmpty();
}

bool TubeItem::isPoured()
{
    return m_poured;
}

bool TubeItem::isDischarged()
{
    return m_discharged;
}


