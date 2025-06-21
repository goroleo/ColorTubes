#include "gameboard.h"
#include <QGuiApplication>

#include "tubeitem.h"
#include "arrowitem.h"
#include "src/ctglobal.h"
#include "src/ctimages.h"
#include "src/game.h"
#include "core/boardmodel.h"
#include "core/solver.h"
#include "core/moveitem.h"


GameBoard::GameBoard(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_model = CtGlobal::board();
    m_tubes = new TubeItems();
    m_arrowIn = new ArrowItem(this);
    m_arrowIn->setArrowId(ArrowItem::CT_ARROW_IN);
    m_arrowOut = new ArrowItem(this);
    m_arrowOut->setArrowId(ArrowItem::CT_ARROW_OUT);

    QObject::connect(&CtGlobal::images(), &CtImages::scaleChanged,
                     this, &GameBoard::onScaleChanged);

    QObject::connect(&CtGlobal::game(), &Game::levelChanged,
                     this, &GameBoard::onLevelChanged);

    QObject::connect(&CtGlobal::game(), &Game::refreshNeeded,
                     this, &GameBoard::onRefresh);

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    update();

    QObject::connect(m_arrowOut, &ArrowItem::shown,
                     this, &GameBoard::onArrowOutShown);
    QObject::connect(m_arrowIn, &ArrowItem::shown,
                     this, &GameBoard::onArrowInShown);

    QObject::connect(qGuiApp, &QGuiApplication::applicationStateChanged,
            this, &GameBoard::onApplicationStateChanged);
}

GameBoard::~GameBoard()
{
    if (!m_tubes->isEmpty())
        qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();
    delete m_tubes;
    delete m_arrowIn;
    delete m_arrowOut;
}

int GameBoard::indexOf(TubeItem * tube)
{
    if (tube) {
        return m_tubes->indexOf(tube);
    } else
        return -1;
}

int GameBoard::maxChildrenZ()
{
    int result = 0;
    for (TubeItem *tube : *m_tubes) {
        if (result < tube->z())
            result = tube->z();
    }
    return result;
}

void GameBoard::childrenZChanged()
{
    bool oldBusy = m_busy;
    m_busy = (maxChildrenZ() != 0);
    if (oldBusy != m_busy) {
        emit busyChanged();
        if (!m_busy) {
            showMove();
            CtGlobal::game().checkSolved();
        }
    }
}

void GameBoard::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    if (oldGeometry.width() != width() || oldGeometry.height() != height())
        rescale();
}

void GameBoard::rescale()
{
    // rescale and replace all tubes
    int cols = m_model->tubesCount() / 2 + m_model->tubesCount() % 2;
    int boardWidth = cols * (80 + spaceX) + spaceX;
    int boardHeight = 400 + 3 * spaceY;

    qreal newScale = qMin(qreal(width() / boardWidth), qreal(height() / boardHeight));

    if (!qFuzzyIsNull(newScale) && !qFuzzyCompare(CtGlobal::images().scale(), newScale)) {
        CtGlobal::images().setScale(newScale);
        showMove();
    } else {
        placeTubes();
        showMove();
    }
}

void GameBoard::update()
{
    if (!m_tubes->isEmpty())
        qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();

    for (int i = 0; i < m_model->tubesCount(); ++i) {
        TubeItem * tube = new TubeItem(this, CtGlobal::board()->tubeAt(i));
        m_tubes->append(tube);
    }
    rescale();
}

void GameBoard::onRefresh()
{
    for (TubeItem *tube : *m_tubes)
        tube->refresh();
    showMove();
}

void GameBoard::onLevelChanged()
{
    update();
}

void GameBoard::onScaleChanged()
{
    placeTubes();
}

void GameBoard::placeTubes()
{
    int columns = m_model->tubesCount() / 2 + m_model->tubesCount() % 2 ;

    qreal leftMargin = CtGlobal::images().scale() * spaceX;
    qreal colSpace = CtGlobal::images().scale() * spaceY;
    qreal rowWidth = width() - leftMargin * 2;
    int   rowColumns = columns;
    qreal rowSpace = (rowWidth - rowColumns * CtGlobal::images().tubeWidth()) / (rowColumns + 1);

    qreal realHeight = colSpace * 3 + CtGlobal::images().tubeHeight() * 2;
    qreal topMargin = qreal (height() - realHeight) / 2;

    int row = 0;
    int col = 0;
    int tubeNumber = 0;

    while (tubeNumber < m_model->tubesCount()) {

        if (tubeNumber == columns) {
            row++;
            rowColumns = m_model->tubesCount() - columns;
            rowSpace = (rowWidth - rowColumns * CtGlobal::images().tubeWidth()) / (rowColumns + 1);
        }

        col = tubeNumber - row * columns;

        m_tubes->at(tubeNumber)->setRegularPosition(
                    QPointF(leftMargin + rowSpace + col * (CtGlobal::images().tubeWidth() + rowSpace),
                            topMargin + colSpace + row * (CtGlobal::images().tubeHeight() + colSpace)));
        tubeNumber ++;
    }

    m_arrowIn->setVisible(false);
    m_arrowOut->setVisible(false);
}

void GameBoard::mousePressEvent(QMouseEvent * event)
{
    clickTube(m_selectedTube);
}

void GameBoard::clickTube(TubeItem * tube)
{
    if (CtGlobal::gameMode() == CT_ASSIST_MODE
            && m_busy)
        return;

    if (tube == nullptr) {
        if (m_selectedTube != nullptr) {
            // qDebug() << "Deselect all tubes";
            m_selectedTube->setSelected(false);
            m_selectedTube = nullptr;
            if (CtGlobal::gameMode() == CT_ASSIST_MODE)
                showMove();
        }
    } else if (m_selectedTube == nullptr) {
        if (tube->canExtractColor()) {
            // qDebug() << "New selection";
            if (CtGlobal::gameMode() == CT_ASSIST_MODE) {
                if (m_arrowOut->connectedTube() == tube) {
                    m_arrowOut->setVisible(false);
                    tube->setShadeBlinked(false);
                    m_arrowIn->setVisible(true);
                    if (m_arrowIn->connectedTube())
                        m_arrowIn->connectedTube()->setShadeBlinked(true);
                    m_arrowIn->startBlink();
                } else {
                    if (CtGlobal::game().level() != 1)
                        emit CtGlobal::game().assistModeError();
                    return;
                }
            }
            tube->setSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    } else { // tube and selectedTube are both assigned

        if (tube == m_selectedTube) {
            // qDebug() << "Disable selection";
            if (CtGlobal::gameMode() == CT_ASSIST_MODE) {
                m_arrowIn->stopBlink();
                if (m_arrowIn->connectedTube())
                    m_arrowIn->connectedTube()->setShadeBlinked(false);
                m_arrowOut->setVisible(true);
            }
            m_selectedTube->setSelected(false);
            m_selectedTube = nullptr;

        } else if (tube->canPutColor(m_selectedTube->currentColor())) {
            // qDebug() << "Put color";
            if (CtGlobal::gameMode() == CT_ASSIST_MODE) {
                if (m_arrowIn->connectedTube() == tube) {
                    if (m_arrowOut->connectedTube())
                        m_arrowOut->connectedTube()->hideShadeImmediately();
                    hideMove();
                } else {
                    if (CtGlobal::game().level() != 1)
                        emit CtGlobal::game().assistModeError();
                    return;
                }
            }
            m_selectedTube->moveColorTo(tube);
            m_selectedTube = nullptr;
        } else {
            // qDebug() << "Change selection";
            if (CtGlobal::gameMode() == CT_ASSIST_MODE) {
                if (m_arrowOut->connectedTube() == tube) {
                    m_arrowOut->setVisible(false);
                    tube->setShadeBlinked(false);
                    m_arrowIn->setVisible(true);
                    if (m_arrowIn->connectedTube())
                        m_arrowIn->connectedTube()->setShadeBlinked(true);
                    m_arrowIn->startBlink();
                } else {
                    if (CtGlobal::game().level() != 1)
                        emit CtGlobal::game().assistModeError();
                    return;
                }
            }
            m_selectedTube->setSelected(false);
            tube->setSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    }

    // show available moves
    if (CtGlobal::gameMode() == CT_PLAY_MODE) {
        for (TubeItem *tube : *m_tubes) {
            if (m_selectedTube != nullptr
                    && m_selectedTube != tube
                    && tube->canPutColor(m_selectedTube->currentColor()))
                tube->showAvailable(true);
            else
                tube->showAvailable(false);
        }
    }
}

void GameBoard::hideSelection()
{
    if (m_selectedTube) {
        m_selectedTube->setSelected(false);
        m_selectedTube = nullptr;
    }
    if (CtGlobal::gameMode() == CT_PLAY_MODE) {
        for (TubeItem *tube : *m_tubes)
            tube->showAvailable(false);
    }
    hideMove();
}

void GameBoard::newMove(TubeItem *from, TubeItem *to)
{
    MoveItem * move = new MoveItem(m_model->getMoveData(*from->model(), *to->model()));
    CtGlobal::game().addMove(move);
}

void GameBoard::showMove()
{
    hideMove();
    if (CtGlobal::gameMode() == CT_ASSIST_MODE) {
        MoveItem * nextMove = CtGlobal::game().nextMove();
        if (nextMove) {
            m_tubes->at(nextMove->tubeFrom())->connectArrow(m_arrowOut);
            m_tubes->at(nextMove->tubeTo())->connectArrow(m_arrowIn);
            m_arrowOut->setVisible(true);
        }
    }
}

void GameBoard::hideMove()
{
    if (m_arrowOut->connectedTube()) {
        m_arrowOut->connectedTube()->removeArrow();
        m_arrowOut->hideImmediately();
    }
    if (m_arrowIn->connectedTube()) {
        m_arrowIn->connectedTube()->removeArrow();
        m_arrowIn->hideImmediately();
    }
}

void GameBoard::onArrowOutShown()
{
    m_arrowOut->startBlink();
    m_arrowOut->connectedTube()->setShadeBlinked(true);
    m_arrowIn->setVisible(true);
}

void GameBoard::onArrowInShown()
{

}

void GameBoard::onApplicationStateChanged()
{
    if (CtGlobal::gameMode() == CT_ASSIST_MODE) {
        if (QGuiApplication::applicationState() == Qt::ApplicationActive)
            showMove();
        else
            hideSelection();
    }
}
