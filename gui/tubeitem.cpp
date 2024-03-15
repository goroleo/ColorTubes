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

    m_back = new BottleLayer(this);
    m_back->setSource(CT_BOTTLE_BACK);

    m_colors = new ColorsLayer(this);

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
        nextFrame();
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
}

void TubeItem::mousePressEvent(QMouseEvent * event)
{
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

void TubeItem::showAvailable(bool value)
{
    if (value) {
        if (m_shade->shade() != 2)
            m_shade->setShadeAfterHiding(2);
    } else
        if (m_shade->shade() != 0)
            m_shade->startHide();
}

void TubeItem::showClosed(bool value)
{
    if (value) {
        if (m_shade->shade() != 3)
            m_shade->setShadeAfterHiding(3);
    } else
        if (m_shade->shade() != 0)
            m_shade->startHide();
}

int TubeItem::tubeIndex()
{
    return m_board->indexOf(this);
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
    return m_currentAngle;
}

void TubeItem::setAngle(qreal newAngle)
{
/*
    // check for external angle changes, don't need inside the app

    newAngle = fmod(newAngle, CT_2PI);
    if (qAbs(newAngle) > CT_PI) {
        newAngle = std::signbit(newAngle)
                ? CT_2PI + newAngle
                : newAngle - CT_2PI;
    }
*/

    if (qFuzzyCompare(m_currentAngle, newAngle))
        return;

    m_currentAngle = newAngle;
    emit angleChanged(newAngle);
}

QPointF TubeItem::pivotPoint()
{
    return m_pivotPoint;
}

void TubeItem::setPosition(const QPointF newPoint)
{
    m_pivotPoint = newPoint;
    m_positionPoint = newPoint;
    placeLayers();
}

void TubeItem::setPivotPoint(QPointF newPoint)
{
    m_pivotPoint = newPoint;

    if (qFuzzyIsNull(m_currentAngle)) {
        QQuickItem::setPosition(m_pivotPoint);
    } else {
        setX(m_pivotPoint.x() - 100 * scale());
        setY(m_pivotPoint.y() + m_verticalShift);
    }
}

void TubeItem::setVerticalShift(qreal yShift)
{
    m_verticalShift = yShift;
    setY(m_pivotPoint.y() + m_verticalShift);
}

void TubeItem::placeLayers()
{
    if (qFuzzyIsNull(m_currentAngle)) {
        m_shade->setY(20 * scale());
        QQuickItem::setPosition(m_pivotPoint);
        setWidth(80 * scale());
        setHeight(200 * scale());
        setClip(true);
    } else {
        setX(m_pivotPoint.x() - 100 * scale());
        setY(m_pivotPoint.y() + m_verticalShift);
        setWidth(0);
        setHeight(0);
        setClip(false);
    }
}

void TubeItem::startAnimation() {
    if (m_timer->isActive())
        m_timer->stop();

    m_moveIncrement.setX((m_endPoint.x() - m_startPoint.x()) / steps);
    m_moveIncrement.setY((m_endPoint.y() - m_startPoint.y()) / steps);
    m_angleIncrement = (m_endAngle - m_startAngle) / steps;
    m_timer->start(TIMER_TICKS);
}

void TubeItem::moveUp()
{
    if (currentStageId != STAGE_REGULAR)
        return;
    currentStageId = STAGE_SELECT;

    m_startPoint = m_positionPoint;
    m_endPoint = QPointF(m_positionPoint.x(), m_positionPoint.y() - 20 * scale());
    m_startAngle = 0;
    m_endAngle = 0;
    steps = STEPS_UP;

    nextStageId = STAGE_SELECT;
    startAnimation();
}

void TubeItem::moveDown()
{
    if (currentStageId != STAGE_SELECT)
        return;

    m_startPoint = m_pivotPoint;
    m_endPoint = m_positionPoint;
    m_startAngle = m_currentAngle;
    m_endAngle = 0;
    steps = STEPS_DOWN;

    nextStageId = STAGE_REGULAR;
    startAnimation();
}

void TubeItem::flyTo(TubeItem * tube)
{
    if (currentStageId != STAGE_SELECT)
        return;

    setClip(false);
    setWidth(0);
    setHeight(0);

    currentStageId = STAGE_FLY;

    m_recipient = tube;
//    tube->addPouringTube(this);
    m_recipient->setShade(0);

    m_startAngle = m_currentAngle;
    m_endAngle = (tube->pivotPoint().x() > m_board->width() / 2)
            ? CtGlobal::TILT_ANGLE[m_model->count()]
            : -CtGlobal::TILT_ANGLE[m_model->count()];

    m_startPoint = m_pivotPoint;
    m_endPoint = QPointF(
                (m_endAngle > 0)
                ? tube->pivotPoint().x() - 20 * scale()
                : tube->pivotPoint().x() + 20 * scale(),
                tube->pivotPoint().y() - 40 * scale());
    steps = STEPS_FLY;
    setZ(m_board->maxChildrenZ() + 1);

    nextStageId = STAGE_POUR_OUT;
    startAnimation();
}

void TubeItem::pourOut()
{
    currentStageId = STAGE_POUR_OUT;

    m_startPoint = m_pivotPoint;
    m_endPoint = m_pivotPoint;
    m_startAngle = m_currentAngle;

    m_endAngle = (m_currentAngle > 0)
            ? CtGlobal::TILT_ANGLE[m_model->count() - m_pouringCells]
            : - CtGlobal::TILT_ANGLE[m_model->count() - m_pouringCells];

    steps = STEPS_POUR_OUT * m_pouringCells;

    if (m_recipient) {
        m_recipient->addPouringTube(this);
        m_recipient->m_pouringCells += m_pouringCells;
    }

    nextStageId = STAGE_RETURN;
    startAnimation();
}

void TubeItem::flyBack()
{
    currentStageId = STAGE_RETURN;

    m_startPoint = m_pivotPoint;
    m_endPoint = m_positionPoint;
    m_startAngle = m_currentAngle;
    m_endAngle = 0;
    steps = STEPS_FLYBACK;

    nextStageId = STAGE_REGULAR;
    startAnimation();
}

void TubeItem::nextStage()
{
    currentStageId = nextStageId;
    switch (currentStageId) {

    case STAGE_POUR_OUT:

        pourOut();
        return;

    case STAGE_RETURN:

        if (m_recipient) {
            for (int i = 0; i < m_pouringCells; i++)
                m_recipient->putColor(m_model->currentColor());
            m_recipient->removePouringTube(this);
            m_recipient = nullptr;
        }

        while (m_pouringCells > 0) {
            m_model->extractColor();
            m_pouringCells --;
        }

        flyBack();
        return;

    case STAGE_REGULAR:

        setZ(0);
        m_shade->setY(20 * scale());
        QQuickItem::setPosition(m_pivotPoint);
        setWidth(80 * scale());
        setHeight(200 * scale());
        setClip(true);

        if (isClosed()) {
            showClosed(true);
            m_cork->setVisible(true);
        }

        if (m_board->selectedTube()
                && canPutColor(m_board->selectedTube()->currentColor())) {
            showAvailable(true);
        }
    }
}

void TubeItem::nextFrame()
{
    if (steps == 0) {
        m_timer->stop();
        if (!qFuzzyIsNull(m_angleIncrement))
            setAngle(m_endAngle);
        setPivotPoint(m_endPoint);
        nextStage();
    } else {
        if (!qFuzzyIsNull(m_angleIncrement))
            setAngle(m_currentAngle + m_angleIncrement);
        setPivotPoint(m_pivotPoint + m_moveIncrement);

        if (currentStageId == STAGE_POUR_OUT && m_recipient) {
            m_recipient->addPouringArea();
        }

        steps --;
    }
}

bool TubeItem::isClosed()
{
    return m_model->isClosed();
}

bool TubeItem::isEmpty()
{
    return m_model->isEmpty();
}

bool TubeItem::isActive()
{
    return !m_model->isClosed()
            && currentStageId <= STAGE_SELECT;
}

bool TubeItem::isPouredIn()
{
    return currentStageId == STAGE_POUR_IN;
}

bool TubeItem::isTilted()
{
    return (currentStageId != STAGE_POUR_IN)
            && (currentStageId > STAGE_SELECT);
}

bool TubeItem::isSelected()
{
    return currentStageId == STAGE_SELECT;
}

void TubeItem::setSelected(bool value)
{
    if (!isActive() || isEmpty() || isSelected() == value)
        return;

    if (value)
        moveUp();
    else
        moveDown();
}

quint8 TubeItem::currentColor()
{
    return m_model->currentColor();
}

bool TubeItem::canExtractColor()
{
    return m_model->canExtractColor();
}

quint8 TubeItem::extractColor()
{
    return m_model->extractColor();
    /*
    quint8 result = m_model->extractColor();
    m_colors->refresh();
    return result;
    */
}

bool TubeItem::canPutColor(quint8 colorNum)
{
    return m_model->canPutColor(colorNum)
            && ((currentStageId <= STAGE_SELECT)
                || (currentStageId == STAGE_POUR_IN && m_fillingColor == colorNum));
}

void TubeItem::putColor(quint8 colorNum)
{
    m_model->putColor(colorNum);
}

void TubeItem::pourOutTo(TubeItem * tube)
{
    if (!tube)
        return;

    m_pouringCells = qMin(m_model->sameColorsCount(),
                        tube->model()->rest());

    nextStageId = STAGE_FLY;
    flyTo(tube);
}

void TubeItem::addPouringTube(TubeItem * tube)
{
    if (!tube)
        return;
    m_pouringTubes ++;
    if (currentStageId != STAGE_POUR_IN) {
        m_colors->resetPouringArea();
        currentStageId = STAGE_POUR_IN;
    }
    m_fillingColor = tube->currentColor();
}

void TubeItem::removePouringTube(TubeItem * tube)
{
    if (!tube)
        return;

    m_pouringTubes --;
    if (m_pouringTubes == 0) {
        m_colors->refresh();
        m_fillingColor = 0;
        nextStageId = STAGE_REGULAR;
        nextStage();
    }
}

qreal TubeItem::getPouringArea()
{
    return m_colors->getPouringArea();
}

void TubeItem::addPouringArea()
{
    m_colors->addPouringArea(CtGlobal::images().colorArea() / STEPS_POUR_OUT, m_fillingColor);
}


