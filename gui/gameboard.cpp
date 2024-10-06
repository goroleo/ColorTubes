#include "gameboard.h"

#include "src/ctglobal.h"
#include "tubeitem.h"
#include "src/ctimages.h"
#include "src/game.h"
#include "core/boardmodel.h"
#include "core/solver.h"

GameBoard::GameBoard(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_model = CtGlobal::board();
    m_tubes = new TubeItems();


    QObject::connect(&CtGlobal::images(), &CtImages::scaleChanged,
                     this, &GameBoard::onScaleChanged);

    QObject::connect(&CtGlobal::game(), &Game::levelChanged,
                     this, &GameBoard::onLevelChanged);

    QObject::connect(&CtGlobal::game(), &Game::needToRefresh,
                     this, &GameBoard::onRefresh);

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);

    update();
}

GameBoard::~GameBoard()
{
    if (!m_tubes->isEmpty())
        qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();
    delete m_tubes;
}

int GameBoard::tubesCount()
{
    return m_model->tubesCount();
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
    for (int i = 0; i < this->children().size(); ++i) {
        if (children().at(i)->inherits("TubeItem")
                && result < ((TubeItem *) children().at(i))->z())
            result = ((TubeItem *) children().at(i))->z();
    }
    return result;
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
    } else {
        placeTubes();
    }
}

void GameBoard::update()
{
    if (!m_tubes->isEmpty())
        qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();

    for (int i = 0; i < tubesCount(); ++i) {
        TubeItem * tube = new TubeItem(this, CtGlobal::board()->tubeAt(i));
        m_tubes->append(tube);
    }
    rescale();
}

void GameBoard::refresh()
{
    for (int i = 0; i < tubesCount(); ++i)
        m_tubes->at(i)->refresh();
}

void GameBoard::onRefresh()
{
    refresh();
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
}

void GameBoard::mousePressEvent(QMouseEvent * event)
{
    clickTube(nullptr);
}

void GameBoard::clickTube(TubeItem * tube)
{
    if (tube == nullptr) {
        if (m_selectedTube != nullptr) {
        // qDebug() << "Disable selection 1";
            m_selectedTube->setSelected(false);
            m_selectedTube = nullptr;
        }
    } else if (m_selectedTube == nullptr) {
        if (tube->canExtractColor()) {
         // qDebug() << "New selection";
            tube->setSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    } else { // tube and selectedTube are both assigned

        if (tube == m_selectedTube) {
         // qDebug() << "Disable selection 2";
            m_selectedTube->setSelected(false);
            m_selectedTube = nullptr;
        } else if (tube->canPutColor(m_selectedTube->currentColor())) {
         // qDebug() << "Put color";
            m_selectedTube->moveColorTo(tube);
            m_selectedTube = nullptr;
        } else {
         // qDebug() << "Change selection";
            m_selectedTube->setSelected(false);
            tube->setSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    }
    showAvailableMoves();
}

void GameBoard::showAvailableMoves()
{
    TubeItem * tube;

    for (int i = 0; i < tubesCount(); ++i) {
        tube = m_tubes->at(i);

        if (m_selectedTube != nullptr
                && m_selectedTube != tube
                && tube->canPutColor(m_selectedTube->currentColor()))
            tube->showAvailable(true);
        else
            tube->showAvailable(false);
    }
}

bool GameBoard::busy()
{
    return maxChildrenZ() != 0;
}

void GameBoard::hideSelection()
{
    if (m_selectedTube)
        clickTube(nullptr);
}

