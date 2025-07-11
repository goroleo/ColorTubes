#include "tubeitem.h"

#include "src/ctglobal.h"
#include "src/ctimages.h"
#include "src/game.h"

#include "core/tubemodel.h"

#include "gameboard.h"
#include "corklayer.h"
#include "bottlelayer.h"
#include "colorslayer.h"
#include "shadelayer.h"
#include "arrowitem.h"

TubeItem::TubeItem(QQuickItem * parent, TubeModel * tm) :
    QQuickItem(parent),
    m_model(tm)
{

    m_board = (GameBoard *) parent;
    if (!m_model)
        m_model = new TubeModel();

    m_shade = new ShadeLayer(this);

    m_back = new BottleLayer(this);
    m_back->setSource(BottleLayer::CT_BOTTLE_BACK);

    m_colors = new ColorsLayer(this);

    m_front = new BottleLayer(this);
    m_front->setSource(BottleLayer::CT_BOTTLE_FRONT);

    m_cork = new CorkLayer(this);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=]() {
        nextFrame();
        update();
    });

    QObject::connect(&CtGlobal::images(), &CtImages::scaleChanged,
                     this, &TubeItem::onScaleChanged);

    QObject::connect(m_model, &TubeModel::stateChanged,
                     this, &TubeItem::onTubeStateChanged);

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    setClosed(m_model->isDone());
    m_shade->setAnimated(true);
    m_cork->setAnimated(true);
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

void TubeItem::setRegularPosition(QPointF newPoint)
{
    m_regularPosition = newPoint;
    m_currentPosition = newPoint;
    regularTube();
}

void TubeItem::setCurrentPosition(QPointF newPoint)
{
    m_currentPosition = newPoint;

    if (qFuzzyIsNull(m_currentAngle)) {
        QQuickItem::setPosition(m_currentPosition);
    } else {
        setX(m_currentPosition.x() - CtGlobal::images().shiftWidth());
        setY(m_currentPosition.y() + m_verticalShift);
    }
}

void TubeItem::setVerticalShift(qreal yShift)
{
    m_verticalShift = yShift;
    setY(m_currentPosition.y() + m_verticalShift);
}

bool TubeItem::isDone()
{
    return m_model->isDone();
}

bool TubeItem::isEmpty()
{
    return m_model->isEmpty();
}

bool TubeItem::isActive()
{
    return !m_model->isDone()
            && currentStageId <= CT_STAGE_SELECT;
}

bool TubeItem::isFlyed()
{
    return (currentStageId > CT_STAGE_SELECT)
            && (currentStageId != CT_STAGE_BUSY);
}

bool TubeItem::isPouredIn()
{
    return currentStageId == CT_STAGE_BUSY;
}

bool TubeItem::isSelected()
{
    return currentStageId == CT_STAGE_SELECT;
}

void TubeItem::setZ(qreal newZ)
{
    QQuickItem::setZ(newZ);
    m_board->childrenZChanged();
}

void TubeItem::refresh()
{
    m_colors->refresh();
    setClosed(m_model->isDone());
}

void TubeItem::setShade(int newShade)
{
    if (m_shade->shade() == newShade || isFlyed())
        return;
    m_shade->setShade(newShade);
}

void TubeItem::setSelected(bool value)
{
    if (!isActive() || m_model->isEmpty())
        return;

    if (value)
        moveUp();
    else
        moveDown();
}

void TubeItem::showAvailable(bool value)
{
    if (m_model->isDone())
        return;

    if (value) {
        if (m_shade->shade() != 2)
            m_shade->setShadeAfterHide(2);
    } else
        if (m_shade->shade() != 0)
            m_shade->startHide();
}

void TubeItem::onTubeStateChanged()
{
    setClosed(m_model->isDone());

    if (m_closed) {
        qDebug() << "Closed tube" << m_board->indexOf(this);
//      CtGlobal::game().checkSolved();
    }
}

void TubeItem::setClosed(bool value)
{
    if (value) {
        if (m_shade->shade() != 3)
            m_shade->setShadeAfterHide(3);
    } else {
        if (m_shade->shade() != 0)
            m_shade->startHide();
    }
    m_cork->setVisible(value);
    m_closed = value;
}

void TubeItem::setShadeBlinked(bool value)
{
    m_shade->setBlinked(value);
}

void TubeItem::hideShadeImmediately()
{
    m_shade->hideImmediately();
}

quint8 TubeItem::currentColor()
{
    return m_model->currentColor();
}

quint8 TubeItem::colorAt(quint8 index)
{
    return m_model->color(index);
}

bool TubeItem::canPutColor(quint8 color)
{
    return (currentStageId == CT_STAGE_DEFAULT
                && m_model->canPutColor(color))

            || (currentStageId == CT_STAGE_BUSY
                && m_fillingColor == color
                && m_pouringCells < m_model->rest());
}

bool TubeItem::canExtractColor()
{
    return m_model->canExtractColor();
}

void TubeItem::moveColorTo(TubeItem * tube)
{
    if (tube)
        flyTo(tube);
}

void TubeItem::onScaleChanged()
{
    regularTube();
}

qreal TubeItem::angle() const
{
    return m_currentAngle;
}

void TubeItem::setAngle(qreal newAngle)
{
/*
    // Angle must be in the range from 0 to +-180 degrees,
    // but going out of range can only happen if the angle is set manually.
    // So checking this is not required inside the app.

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

void TubeItem::startAnimation() {
    if (m_timer->isActive())
        m_timer->stop();

    m_moveIncrement.setX( (m_endPoint.x() - m_startPoint.x()) / m_steps);
    m_moveIncrement.setY( (m_endPoint.y() - m_startPoint.y()) / m_steps);
    m_angleIncrement = ( m_endAngle - m_startAngle ) / m_steps;
    m_timer->start(CT_TIMER_TICKS);
}

void TubeItem::nextFrame()
{
    if (m_steps > 0) {

        if (currentStageId == CT_STAGE_POUR_OUT) {
            setAngle(CtGlobal::images().tiltAngle(m_endAngleNumber + m_steps)
                     * CtGlobal::sign(m_currentAngle));
            if (m_recipient)
                m_recipient->addFillArea();
        } else {
            if (!qFuzzyIsNull(m_angleIncrement))
                setAngle(m_currentAngle + m_angleIncrement);
        }

        setCurrentPosition(m_currentPosition + m_moveIncrement);
        m_steps --;

    } else {
        m_steps = 0;
        m_timer->stop();

        if (!qFuzzyIsNull(m_angleIncrement))
            setAngle(m_endAngle);

        setCurrentPosition(m_endPoint);
        nextStage();
    }
}

void TubeItem::nextStage()
{
    currentStageId = nextStageId;

    switch (currentStageId) {
    case CT_STAGE_DEFAULT:
        regularTube();
        return;

    case CT_STAGE_POUR_OUT:
        pourOut();
        return;

    case CT_STAGE_BACK:
        flyBack();
        return;
    }
}

void TubeItem::regularTube()
{
    currentStageId = CT_STAGE_DEFAULT;

    QQuickItem::setPosition(m_regularPosition);

    setWidth(CtGlobal::images().tubeWidth());
    setHeight(CtGlobal::images().tubeFullHeight());
    m_shade->setY(CtGlobal::images().shiftHeight());
    setClip(true);
    setZ(0);

    if (CtGlobal::gameMode() == CT_PLAY_MODE) {
        if (m_board->selectedTube()
                && canPutColor(m_board->selectedTube()->currentColor()))
            showAvailable(true);
    }
}

void TubeItem::moveUp()
{
    if (currentStageId != CT_STAGE_DEFAULT)
        return;
    currentStageId = CT_STAGE_SELECT;

    m_startPoint = m_regularPosition;
    m_endPoint = QPointF(m_regularPosition.x(),
                         m_regularPosition.y() - CtGlobal::images().shiftHeight());
    m_startAngle = 0.0;
    m_endAngle = 0.0;
    m_steps = CT_TUBE_STEPS_UP;

    nextStageId = CT_STAGE_SELECT;
    startAnimation();
}

void TubeItem::moveDown()
{
    if (currentStageId != CT_STAGE_SELECT)
        return;

    m_startPoint = m_currentPosition;
    m_endPoint = m_regularPosition;
    m_startAngle = m_currentAngle;
    m_endAngle = 0.0;
    m_steps = CT_TUBE_STEPS_DOWN;

    nextStageId = CT_STAGE_DEFAULT;
    startAnimation();
}

void TubeItem::flyTo(TubeItem * tubeTo)
{
    if (currentStageId != CT_STAGE_SELECT
            || !tubeTo)
        return;

    setClip(false);
    setWidth(0);
    setHeight(0);

// recipient
    m_recipient = tubeTo;
    m_recipient->setShade(0);
    m_pouringCells = qMin(m_model->currentColorCount(), tubeTo->model()->rest());
    m_recipient->connectTube(this);

// angles
    m_startAngle = m_currentAngle;
    m_endAngle = CtGlobal::images().tiltAngle(CT_TUBE_STEPS_POUR * m_model->count());

// rotation direction
    bool clockWise = this->m_currentPosition.x() < tubeTo->m_currentPosition.x();
    if (clockWise) {
        if (tubeTo->m_currentPosition.x() + CtGlobal::images().tubeWidth() / 2.0
                - CtGlobal::images().tubeRotationWidth() < 0)
            clockWise = false;
    } else {
        if (tubeTo->m_currentPosition.x() + CtGlobal::images().tubeWidth() / 2.0
                + CtGlobal::images().tubeRotationWidth() > m_board->width())
            clockWise = true;
    }
    if (!clockWise)
        m_endAngle *= -1;

// coordinates
    m_startPoint = m_currentPosition;
    m_endPoint = QPointF(tubeTo->m_currentPosition.x()
                             - CtGlobal::images().vertex(5).x() * CtGlobal::sign(m_endAngle),
                         tubeTo->m_currentPosition.y()
                             - CtGlobal::images().shiftHeight() * 2);

    m_steps = CT_TUBE_STEPS_FLY;
    nextStageId = CT_STAGE_POUR_OUT;

    setZ(m_board->maxChildrenZ() + 1);
    startAnimation();
}

void TubeItem::pourOut()
{
    if (m_recipient)
        m_recipient->m_pouredTubes ++;

    m_startPoint = m_currentPosition;
    m_endPoint = m_currentPosition;

    m_startAngle = m_currentAngle;
    m_endAngleNumber = (m_model->count() - m_pouringCells) * CT_TUBE_STEPS_POUR;
    m_endAngle = CtGlobal::images().tiltAngle(m_endAngleNumber)
            * CtGlobal::sign(m_currentAngle);

    m_steps = CT_TUBE_STEPS_POUR * m_pouringCells;

    nextStageId = CT_STAGE_BACK;
    startAnimation();
}

void TubeItem::flyBack()
{
    if (m_recipient)
        m_recipient->removeConnectedTube(this);

    m_startPoint = m_currentPosition;
    m_endPoint = m_regularPosition;
    m_startAngle = m_currentAngle;
    m_endAngle = 0.0;
    m_steps = CT_TUBE_STEPS_BACK;

    nextStageId = CT_STAGE_DEFAULT;
    startAnimation();
}

void TubeItem::connectTube(TubeItem * tubeFrom)
{
    if (!tubeFrom)
        return;
    m_connectedTubes ++;
    m_pouringCells += tubeFrom->m_pouringCells;
    if (currentStageId != CT_STAGE_BUSY) {
        m_fillingColor = tubeFrom->currentColor();
        m_colors->resetFillArea();
        currentStageId = CT_STAGE_BUSY;
    }
}

void TubeItem::removeConnectedTube(TubeItem * tubeFrom)
{
    if (!tubeFrom)
        return;
    if (tubeFrom->m_recipient != this)
        return;

    m_board->newMove(tubeFrom, this);
    for (int i = 0; i < tubeFrom->m_pouringCells; i++) {
        m_model->putColor(tubeFrom->model()->extractColor());
    }

    m_pouringCells -= tubeFrom->m_pouringCells;
    tubeFrom->m_pouringCells = 0;
    tubeFrom->m_recipient = nullptr;

    m_connectedTubes --;
    m_pouredTubes --;

    if (m_pouredTubes == 0) {
        m_colors->addFillArea(0, 0, m_fillingColor);
    }

    if (m_connectedTubes == 0) {
        m_fillingColor = 0;
        m_colors->refresh();
        regularTube();
    }
}

void TubeItem::addFillArea()
{
    m_colors->addFillArea(CtGlobal::images().colorArea() / CT_TUBE_STEPS_POUR, m_fillingColor);
}

void TubeItem::connectArrow(ArrowItem * arrow)
{
    if (arrow) {
        m_arrow = arrow;
        m_arrow->setConnectedTube(this);
        m_arrow->setX(m_regularPosition.x());
        m_arrow->setY(m_regularPosition.y() - CtGlobal::images().shiftArrow());
        m_arrow->setVisible(false);
        m_shade->setBlinked(false);
        if (m_arrow == m_board->m_arrowOut)
            m_shade->setShade(1);
        else if (m_arrow == m_board->m_arrowIn)
            m_shade->setShade(2);
    }
}

void TubeItem::removeArrow()
{
    if (m_arrow) {
        m_shade->setShade(0);
        m_shade->setBlinked(false);
        m_arrow->setVisible(false);
        m_arrow->setConnectedTube(nullptr);
        m_arrow = nullptr;
    }
}

