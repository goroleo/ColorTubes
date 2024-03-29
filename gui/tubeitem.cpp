#include "tubeitem.h"

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

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
                     this, SLOT(onScaleChanged()));

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=]() {
        currentFrame();
        update();
    });
}

TubeItem::~TubeItem()
{
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

int TubeItem::tubeIndex()
{
    return m_board->indexOf(this);
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

quint8 TubeItem::currentColor()
{
    return m_model->currentColor();
}

quint8 TubeItem::getColor(quint8 index)
{
    return m_model->color(index);
}

bool TubeItem::canExtractColor()
{
    return m_model->canExtractColor();
}

quint8 TubeItem::extractColor()
{
    quint8 result = m_model->extractColor();
    m_colors->refresh();
    return result;
}

bool TubeItem::canPutColor(quint8 colorNum)
{
    return (currentStageId == STAGE_REGULAR
                && m_model->canPutColor(colorNum))

            || (currentStageId == STAGE_POUR_IN
                && m_fillingColor == colorNum
                && m_pouringCells < m_model->rest());
}

void TubeItem::putColor(quint8 colorNumber)
{
    m_model->putColor(colorNumber);
    m_colors->refresh();
}

void TubeItem::moveColorTo(TubeItem * tube)
{
    if (tube)
        flyTo(tube);
}

qreal TubeItem::scale() const
{
    return CtGlobal::images().scale();
}

void TubeItem::onScaleChanged()
{
    regularTube();
}

int TubeItem::shade()
{
    return m_shade->shade();
}

void TubeItem::setShade(int newShade)
{
    if (m_shade->shade() == newShade || isTilted())
        return;

    m_shade->setShade(newShade);
    if (newShade != 0)
        m_shade->startShow();
    emit shadeChanged(newShade);
}

void TubeItem::showSelected(bool value)
{
    if (!isActive() || isEmpty())
        return;

    if (value)
        moveUp();
    else
        moveDown();
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
        m_cork->setVisible(true);
    } else {
        if (m_shade->shade() != 0)
            m_shade->startHide();
        m_cork->setVisible(false);
    }
}

qreal TubeItem::angle() const
{
    return m_currentAngle;
}

void TubeItem::setAngle(qreal newAngle)
{
/*
 * checking for external angle changes, doesn't need inside the app

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
    m_position = newPoint;
    regularTube();
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

void TubeItem::startAnimation() {
    if (m_timer->isActive())
        m_timer->stop();

    m_moveIncrement.setX((m_endPoint.x() - m_startPoint.x()) / steps);
    m_moveIncrement.setY((m_endPoint.y() - m_startPoint.y()) / steps);
    m_angleIncrement = (m_endAngle - m_startAngle) / steps;
    m_timer->start(TIMER_TICKS);
}

void TubeItem::currentFrame()
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

void TubeItem::nextStage()
{
    currentStageId = nextStageId;

    switch (currentStageId) {
    case STAGE_REGULAR:
        regularTube();
        return;

    case STAGE_POUR_OUT:
        pourOut();
        return;

    case STAGE_FLY_BACK:
        flyBack();
        return;
    }
}

void TubeItem::regularTube()
{
    currentStageId = STAGE_REGULAR;

    setZ(0);
    m_shade->setY(20 * scale());
    QQuickItem::setPosition(m_pivotPoint);
    setWidth(80 * scale());
    setHeight(200 * scale());
    setClip(true);
    if (m_board->selectedTube()
            && canPutColor(m_board->selectedTube()->currentColor()))
        showAvailable(true);

    if (isClosed()) {
        showClosed(true);
        m_board->checkSolved();
        qDebug() << "Closed tube" << tubeIndex() << "| Solved:" << m_board->isSolved();
    }

    if (m_board->isSolved()) {
        if (m_board->maxChildrenZ() == 0) {
            qDebug() << "All on their places. Tube" << tubeIndex();
            qDebug() << "!!! SOLVED !!!";
        } else {
            qDebug() << "Not on places yet... Tube" << tubeIndex();
        }
    }
}

void TubeItem::moveUp()
{
    if (currentStageId != STAGE_REGULAR)
        return;
    currentStageId = STAGE_SELECT;

    m_startPoint = m_position;
    m_endPoint = QPointF(m_position.x(), m_position.y() - 20 * scale());
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
    m_endPoint = m_position;
    m_startAngle = m_currentAngle;
    m_endAngle = 0;
    steps = STEPS_DOWN;

    nextStageId = STAGE_REGULAR;
    startAnimation();
}

void TubeItem::flyTo(TubeItem * tubeTo)
{
    if (currentStageId != STAGE_SELECT)
        return;

    setClip(false);
    setWidth(0);
    setHeight(0);

    m_recipient = tubeTo;
    m_recipient->setShade(0);
    m_pouringCells = qMin(m_model->sameColorCount(),
                        tubeTo->model()->rest());
    if (m_recipient)
        m_recipient->connectPouringTube(this);

    m_startAngle = m_currentAngle;
    m_endAngle = ((tubeTo->pivotPoint().x() + 40 * scale()) >= m_board->width() / 2)
            ? CtGlobal::TILT_ANGLE[m_model->count()]
            : -CtGlobal::TILT_ANGLE[m_model->count()];

    m_startPoint = m_pivotPoint;
    m_endPoint = QPointF((m_endAngle > 0)
                ? tubeTo->pivotPoint().x() - 20 * scale()
                : tubeTo->pivotPoint().x() + 20 * scale(),
                tubeTo->pivotPoint().y() - 40 * scale());
    steps = STEPS_FLY;
    setZ(m_board->maxChildrenZ() + 1);

    nextStageId = STAGE_POUR_OUT;
    startAnimation();
}

void TubeItem::pourOut()
{
    if (m_recipient)
        m_recipient->m_pouredTubes ++;

    m_startPoint = m_pivotPoint;
    m_endPoint = m_pivotPoint;

    m_startAngle = m_currentAngle;
    m_endAngle = (m_currentAngle > 0)
            ? CtGlobal::TILT_ANGLE[m_model->count() - m_pouringCells]
            : - CtGlobal::TILT_ANGLE[m_model->count() - m_pouringCells];

    steps = STEPS_POUR_OUT * m_pouringCells;

    nextStageId = STAGE_FLY_BACK;
    startAnimation();
}

void TubeItem::flyBack()
{
    if (m_recipient)
        m_recipient->removeConnectedTube(this);

    m_startPoint = m_pivotPoint;
    m_endPoint = m_position;
    m_startAngle = m_currentAngle;
    m_endAngle = 0;
    steps = STEPS_FLYBACK;

    nextStageId = STAGE_REGULAR;
    startAnimation();
}

void TubeItem::connectPouringTube(TubeItem * tubeFrom)
{
    if (!tubeFrom)
        return;
    m_connectedTubes ++;
    m_pouringCells += tubeFrom->m_pouringCells;
    if (currentStageId != STAGE_POUR_IN) {
        m_fillingColor = tubeFrom->currentColor();
        m_colors->resetPourArea();
        currentStageId = STAGE_POUR_IN;
    }
}

void TubeItem::removeConnectedTube(TubeItem * tubeFrom)
{
    if (!tubeFrom)
        return;
    if (tubeFrom->m_recipient != this)
        return;

    for (int i = 0; i < tubeFrom->m_pouringCells; i++) {
        m_model->putColor(tubeFrom->model()->extractColor());
    }
    m_pouringCells -= tubeFrom->m_pouringCells;
    tubeFrom->m_pouringCells = 0;
    tubeFrom->m_recipient = nullptr;

    m_connectedTubes --;
    m_pouredTubes --;

    if (m_pouredTubes == 0) {
        m_colors->addPourArea(0, 0, m_fillingColor);
    }

    if (m_connectedTubes == 0) {
        m_fillingColor = 0;
        m_colors->refresh();
        regularTube();
    }
}

void TubeItem::addPouringArea()
{
    m_colors->addPourArea(CtGlobal::images().colorArea() / STEPS_POUR_OUT, m_fillingColor);
}


