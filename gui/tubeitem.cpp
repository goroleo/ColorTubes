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
    m_shade->setVisible(true);
    m_shade->setShade(0);
    m_shade->startShow();

    back = new BottleLayer(this);
    back->setSource(CT_BOTTLE_BACK);
    back->setVisible(true);

    colors = new ColorsLayer(this);
    colors->setModel(m_model);

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
//    qDebug() << event->pos();
//    qDebug() << this->z();
//    this->setZ(100);
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
    placeLayers();

    emit angleChanged(newAngle);
}

void TubeItem::setPivotPoint(QPointF newPoint)
{
    m_pivotPoint = newPoint;
    placeLayers();
}

void TubeItem::placeLayers()
{
    m_shade->setY(20 * scale());
    back->setX(0);
    colors->setX(0);
    front->setX(0);
    this->setY(m_pivotPoint.y());

    if (qFuzzyIsNull(m_angle)) {

        m_shade->setX(0);
        colors->setY(m_shade->y());
        cork->setX(0);
        this->setX(m_pivotPoint.x());

        setWidth(80 * scale());
        setHeight(200 * scale());
        setClip(true);

    } else {

        m_shade->setX(100 * scale());
        colors->setY(0);
        cork->setX(m_shade->x());
        this->setX(m_pivotPoint.x() - 100 * scale());

        setWidth(0);
        setHeight(0);
        setClip(false);
    }
}

void TubeItem::rotate()
{
    m_angleIncrement = std::copysign(0.5 * CT_DEG2RAD, m_angleIncrement);
    setZ(m_board->maxChildrenZ() + 1); // above all other tubes
//    qDebug() << z();
    m_rotateTimer->start(1);
}

void TubeItem::addAngleIncrement()
{
    setAngle(m_angle + m_angleIncrement);
    if (abs(m_angle) >= 125 * CT_DEG2RAD)
        m_angleIncrement = - m_angle / 30.0;
    if (qFuzzyIsNull(m_angle)) {
        setAngle(0);
        m_rotateTimer->stop();
        setZ(0);
    }
}


