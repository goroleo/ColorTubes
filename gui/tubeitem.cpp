#include "tubeitem.h"

#include "src/ctglobal.h"
#include "src/ctimages.h"

#include "gameboard.h"
#include "core/tubemodel.h"

#include "corklayer.h"
#include "bottlelayer.h"
#include "colorslayer.h"
#include "shadelayer.h"

TubeItem::TubeItem(QQuickItem * parent, TubeModel * tm) :
    QQuickItem(parent),
    m_model(tm)
{
    m_board = (GameBoard *) parent;

    m_shade = new ShadeLayer(this);
    m_shade->setShade(0);

    m_back = new BottleLayer(this);
    m_back->setSource(BottleLayer::CT_BOTTLE_BACK);

    m_colors = new ColorsLayer(this);

    m_front = new BottleLayer(this);
    m_front->setSource(BottleLayer::CT_BOTTLE_FRONT);

    m_cork = new CorkLayer(this);
    m_cork->setVisible(false);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
                     this, SLOT(onScaleChanged()));

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, [=]() {
        nextFrame();
        update();
    });

    if (tm) {
        QObject::connect(tm, &TubeModel::stateChanged,
                         this, &TubeItem::onTubeStateChanged);
        setClosed(tm->isDone());
    }
}

TubeItem::~TubeItem()
{
    delete m_timer;
    delete m_cork;
    delete m_front;
    delete m_colors;
    delete m_back;
    delete m_shade;
    qDebug() << "Tube" << tubeIndex() << "deleted";
}

void TubeItem::mousePressEvent(QMouseEvent * event)
{
    m_board->clickTube(this);
}

int TubeItem::tubeIndex()
{
    return m_board->indexOf(this);
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
    if (newShade != 0)
        m_shade->startShow();
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
        qDebug() << "Closed tube" << tubeIndex();
        if (m_board->isSolved()) {
            qDebug() << "!!! SOLVED !!!";
            emit m_board->solved();
        }
    }
}

void TubeItem::setClosed(bool value)
{
    if (value == m_closed)
        return;
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

quint8 TubeItem::currentColor()
{
    return m_model->currentColor();
}

quint8 TubeItem::colorAt(quint8 index)
{
    return m_model->color(index);
}

bool TubeItem::canPutColor(quint8 colorNum)
{
    return (currentStageId == CT_STAGE_DEFAULT
                && m_model->canPutColor(colorNum))

            || (currentStageId == CT_STAGE_BUSY
                && m_fillingColor == colorNum
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

qreal TubeItem::scale() const
{
    return CtGlobal::images().scale();
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

    m_moveIncrement.setX( (m_endPoint.x() - m_startPoint.x()) / steps);
    m_moveIncrement.setY( (m_endPoint.y() - m_startPoint.y()) / steps);
    m_angleIncrement = ( m_endAngle - m_startAngle ) / steps;
    m_timer->start(CT_TIMER_TICKS);
}

void TubeItem::nextFrame()
{
    if (steps > 0) {

        if (currentStageId != CT_STAGE_POUR_OUT) {
            if (!qFuzzyIsNull(m_angleIncrement))
                setAngle(m_currentAngle + m_angleIncrement);
        } else {
            setAngle(CtGlobal::images().tiltAngle(m_endAngleNumber + steps)
                     * CtGlobal::sign(m_currentAngle));
            if (m_recipient)
                m_recipient->addPouringArea();
        }

        setCurrentPosition(m_currentPosition + m_moveIncrement);
        steps --;

    } else {
        steps = 0;
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

    case CT_STAGE_FLY_BACK:
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
    setClip(true);
    setZ(0);
    m_shade->setY(CtGlobal::images().shiftHeight());

    if (m_board->selectedTube()
            && canPutColor(m_board->selectedTube()->currentColor()))
        showAvailable(true);
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
    steps = CT_TUBE_STEPS_UP;

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
    steps = CT_TUBE_STEPS_DOWN;

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
    m_pouringCells = qMin(m_model->sameColorCount(), tubeTo->model()->rest());
    m_recipient->connectTube(this);

// angles
    m_startAngle = m_currentAngle;
    m_endAngle = CtGlobal::images().tiltAngle(CT_TUBE_STEPS_POUR * m_model->count());

// rotation direction
    bool clockWise = this->m_currentPosition.x() < tubeTo->m_currentPosition.x();
    if (clockWise) {
        if (tubeTo->m_currentPosition.x() + CtGlobal::tubeWidth() / 2.0
                - CtGlobal::images().tubeRotationWidth() < 0)
            clockWise = false;
    } else {
        if (tubeTo->m_currentPosition.x() + CtGlobal::tubeWidth() / 2.0
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

    steps = CT_TUBE_STEPS_FLY;
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

    steps = CT_TUBE_STEPS_POUR * m_pouringCells;

    nextStageId = CT_STAGE_FLY_BACK;
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
    steps = CT_TUBE_STEPS_BACK;

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
        m_colors->resetPourArea();
        currentStageId = CT_STAGE_BUSY;
    }
}

void TubeItem::removeConnectedTube(TubeItem * tubeFrom)
{
    if (!tubeFrom)
        return;
    if (tubeFrom->m_recipient != this)
        return;

    m_board->addNewMove(tubeFrom, this);
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
    m_colors->addPourArea(CtGlobal::images().colorArea() / CT_TUBE_STEPS_POUR, m_fillingColor);
}



