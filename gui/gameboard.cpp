#include "gameboard.h"

#include "src/ctglobal.h"
#include "src/tubeimages.h"
#include "src/game.h"
#include "core/boardmodel.h"

#include "tubeitem.h"

GameBoard::GameBoard(QQuickItem *parent) :
    QQuickItem(parent)
{
    m_model = CtGlobal::game().boardModel();
    m_tubes = new QVector<TubeItem *>;

    for (int i=0; i < m_model->tubesCount(); ++i) {
        TubeItem * tube = new TubeItem(this, CtGlobal::game().boardModel()->getTube(i));
        m_tubes->append(tube);
    }

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    setAcceptedMouseButtons(Qt::AllButtons);
    setFlag(ItemAcceptsInputMethod, true);
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
    for (int i = 0; i < this->children().size(); ++i)
    {
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
    {
        // rescale and replace all tubes

        int cols = m_model->tubesCount() / 2 + m_model->tubesCount() % 2;
        int boardWidth = cols * (80 + spaceX) + spaceX;
        int boardHeight = 400 + 3 * spaceY;

        qreal newScale = qMin(qreal(width() / boardWidth), qreal(height() / boardHeight));

        if (!qFuzzyIsNull(newScale) && !qFuzzyCompare(scale(), newScale)) {
            CtGlobal::images().setScale(newScale);
        } else {
            placeTubes();
        }
    }
}

qreal GameBoard::scale() const
{
    return CtGlobal::images().scale();
}

void GameBoard::onScaleChanged()
{
    placeTubes();
}

void GameBoard::placeTubes()
{
    int columns = m_model->tubesCount() / 2 + m_model->tubesCount() % 2 ;

    qreal leftMargin = scale() * spaceX;
    qreal rowWidth = width() - leftMargin * 2;
    qreal tubeWidth = scale() * 80;
    int   rowColumns = columns;
    qreal rowSpace = (rowWidth - rowColumns * tubeWidth) / (rowColumns + 1);

    qreal realHeight = scale() * (400 + 3 * spaceY);
    qreal topMargin = qreal (height() - realHeight) / 2;

    int row = 0;
    int col = 0;
    int tubeNumber = 0;

    while (tubeNumber < m_model->tubesCount()) {

        if (tubeNumber == columns) {
            row = 1;
            rowColumns = m_model->tubesCount() - columns;
            rowSpace = (rowWidth - rowColumns * tubeWidth) / (rowColumns + 1);
        }

        col = tubeNumber - row * columns;

        m_tubes->at(tubeNumber)->setPosition(
                    QPointF(leftMargin + rowSpace + col * (tubeWidth + rowSpace),
                            topMargin + scale() * (spaceY + row * (200 + spaceY))));
        tubeNumber ++;
    }
}

void GameBoard::mousePressEvent(QMouseEvent* event)
{
    clickTube(nullptr);
}

void GameBoard::clickTube(TubeItem * tube)
{
    if (tube == nullptr) {
        if (m_selectedTube != nullptr) {
        // qDebug() << "disable selection 1";
            m_selectedTube->showSelected(false);
            m_selectedTube = nullptr;
        }
    } else if (m_selectedTube == nullptr) {
        if (tube->canExtractColor()) {
         // qDebug() << "new selection";
            tube->showSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    } else {
        if (tube == m_selectedTube) {
         // qDebug() << "disable selection 2";
            m_selectedTube->showSelected(false);
            m_selectedTube = nullptr;
        } else if (tube->canPutColor(m_selectedTube->currentColor())) {
         // qDebug() << "put color";
            m_selectedTube->moveColorTo(tube);
            m_selectedTube = nullptr;
        } else {
         // qDebug() << "change selection";
            m_selectedTube->showSelected(false);
            tube->showSelected(true);
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

    int i = 0;
    while (i < tubesCount()) {
        tube = m_tubes->at(i);

        if (m_selectedTube != nullptr
                && m_selectedTube != tube
                && tube->canPutColor(m_selectedTube->currentColor()))
            tube->showAvailable(true);
        else {
            if (!tube->isClosed())
                tube->showAvailable(false);
        }
        i++;
    }
}

void GameBoard::moveColor(TubeItem * tubeFrom, TubeItem * tubeTo)
{
    if (tubeFrom == nullptr || tubeTo == nullptr)
        return;
    tubeFrom->moveColorTo(tubeTo);
}

void GameBoard::moveColor(int tubeFromIndex, int tubeToIndex)
{
    if (tubeFromIndex < 0 || tubeFromIndex >= m_model->tubesCount()
            || tubeToIndex < 0 || tubeToIndex >= m_model->tubesCount())
        return;

    moveColor(m_tubes->at(tubeFromIndex), m_tubes->at(tubeToIndex));
}

bool GameBoard::checkSolved()
{
    m_solved = m_model->isSolved();
    return m_solved;
}

