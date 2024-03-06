#include "tubeitem.h"

#include <QSGClipNode>

#include "src/ctglobal.h"
#include "src/tubeimages.h"

#include "core/tubemodel.h"

#include "bottlelayer.h"
#include "corklayer.h"
#include "colorslayer.h"
#include "shadelayer.h"
#include "flowlayer.h"

#include "gameboard.h"

TubeItem::TubeItem(QQuickItem *parent, TubeModel * tm) :
    QQuickItem(parent),
    m_model(tm)
{
    m_board = (GameBoard *) parent;

    m_shade = new ShadeLayer(this);
    m_shade->setShade(0);
    m_shade->setY(20 * scale());

    m_back = new BottleLayer(this);
    m_back->setSource(CT_BOTTLE_BACK);

    m_flow = new FlowLayer(this, tm);
    m_flow->setVisible(false);

    m_colors = new ColorsLayer(this, tm);

    m_front = new BottleLayer(this);
    m_front->setSource(CT_BOTTLE_FRONT);

    m_cork = new CorkLayer(this);
    m_cork->setVisible(false);

    placeLayers();

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=]() {
        addAngleIncrement();
        update();
    });
}

TubeItem::~TubeItem()
{
    qDebug() << "TubeItem deleted";

    delete m_timer;
    delete m_cork;
    delete m_front;
    delete m_colors;
    delete m_back;
    delete m_shade;
    delete m_flow;
}

void TubeItem::mousePressEvent(QMouseEvent * event)
{
//    QQuickItem::mousePressEvent(event);
    m_board->clickTube(this);
}

int TubeItem::shade()
{
    return m_shade->shade();
}

void TubeItem::setShade(int newShade)
{
    if (m_shade->shade() == newShade)
        return;

    m_shade->setShade(newShade);
    if (newShade != 0)
        m_shade->startShow();
    emit shadeChanged(newShade);
}

qreal TubeItem::scale() const
{
    return CtGlobal::images().scale();
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
    return m_pivotPoint;
}

void TubeItem::setPivotPoint(QPointF newPoint)
{
    m_pivotPoint = newPoint;
    placeLayers();
}

void TubeItem::setYShift(qreal ys)
{
    m_yShift = ys;
    setY(m_pivotPoint.y() + m_yShift);
}

void TubeItem::placeLayers()
{
    if (qFuzzyIsNull(m_angle)) {
        m_shade->setY(20 * scale());
        m_discharged = false;
        setPosition(m_pivotPoint);
        setWidth(80 * scale());
        setHeight(200 * scale());
        setClip(true);
    } else {
        m_discharged = true;
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
    m_timer->start(10);
}

int steps = 20;

void TubeItem::flyTo(TubeItem * tube)
{

    if (!isSelected() || (tube == nullptr)
            || tube->isSelected()
            || tube->isCLosed()
            || tube->isDischarged())
        return;

    startPoint = QPointF(x(), y());
    endPoint = QPointF(
                tube->pivotPoint().x() + 20 * scale(),
                tube->pivotPoint().y() + 20 * scale());

    startAngle = 0;
    endAngle = 26.9970  * CT_DEG2RAD;



}

void TubeItem::flyBack()
{
    startPoint = QPointF(x(), y());
    endPoint = pivotPoint();
    startAngle = m_angle;
    endAngle = 0;

    steps = 20;
}


void TubeItem::addAngleIncrement()
{
    setAngle(m_angle + m_angleIncrement);
    if (abs(m_angle) >= 118.6105 * CT_DEG2RAD)
        m_angleIncrement = - m_angle / 30.0;
    if (qFuzzyIsNull(m_angle)) {
        setAngle(0);

//------------
//        qreal xShift = std::copysign(20*scale(), m_angleIncrement);
//        setPivotPoint(QPointF(m_pivotPoint.x() - xShift, m_pivotPoint.y() + 20 * scale()));
//        m_flow->setX(0);

        m_timer->stop();
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

bool TubeItem::isSelected()
{
    return m_selected;
}

void TubeItem::setSelected(bool newSelected)
{
    if (newSelected == m_selected)
        return;
    m_selected = newSelected;
//    setShade(0);
    setYShift((m_selected)? -20 * scale() : 0);
}

