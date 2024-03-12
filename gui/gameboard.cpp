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
    for (int i=m_model->tubesCount(); i > 0; --i) {
        delete m_tubes->at(i-1);
    }
    delete m_tubes;
}

qreal GameBoard::scale() const
{
    return CtGlobal::images().scale();
}

void GameBoard::onScaleChanged()
{
    placeTubes();
}

void GameBoard::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    if (oldGeometry.width() != width() || oldGeometry.height() != height())
        reScale();
}

void GameBoard::mousePressEvent(QMouseEvent* event)
{
    clickTube(nullptr);
}

void GameBoard::reScale()
{
    qreal newScale = 0.0;

    int cols = m_model->tubesCount() / 2;
    if (cols * 2 < m_model->tubesCount())
        cols ++;

    int tubesWidth = cols * (80 + spaceX) + spaceX;
    int tubesHeight = 400 + 3 * spaceY;

    newScale = qMin(qreal(width() / tubesWidth), qreal(height() / tubesHeight));

    if (!qFuzzyIsNull(newScale) && !qFuzzyCompare(scale(), newScale))
    {
        CtGlobal::images().setScale(newScale);
    } else {
        placeTubes();
    }
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

int GameBoard::indexOf(TubeItem * tube)
{
    if (tube) {
        return m_tubes->indexOf(tube);
    } else
        return -1;
}

void GameBoard::clickTube(TubeItem * tube)
{
    if (tube == nullptr) {
        if (m_selectedTube != nullptr) {
            // qDebug() << "disable selection 1";
            m_selectedTube->setSelected(false);
            m_selectedTube = nullptr;
        }
    } else if (m_selectedTube == nullptr) {
        if (tube->canExtractColor()) {
            // qDebug() << "new selection";
            tube->setSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    } else {
        if (tube == m_selectedTube) {
            // qDebug() << "disable selection 2";
            m_selectedTube->setSelected(false);
            m_selectedTube = nullptr;
        } else if (tube->canPutColor(m_selectedTube->currentColor())) {
            // qDebug() << "put color";
            m_selectedTube->dropColorsTo(tube);
            m_selectedTube = nullptr;
        } else {
            // qDebug() << "change selection";
            m_selectedTube->setSelected(false);
            tube->setSelected(true);
            if (tube->isSelected())
                m_selectedTube = tube;
            else
                m_selectedTube = nullptr;
        }
    }

    // show possible moves, hide previous
    int i = 0;
    while (i < m_model->tubesCount()) {
        tube = m_tubes->at(i);

        if (m_selectedTube != nullptr
                && m_selectedTube != tube
                && tube->isActive()
                && tube->canPutColor(m_selectedTube->currentColor()))
            tube->setShade(2);
        else {
            if (!tube->isClosed())
                tube->setShade(0);
        }
        i++;
    }
}

void GameBoard::moveColor(TubeItem * tubeFrom, TubeItem * tubeTo)
{
    if (tubeFrom == nullptr || tubeTo == nullptr)
        return;

    tubeFrom->dropColorsTo(tubeTo);
}

void GameBoard::moveColor(int tubeFromIndex, int tubeToIndex)
{
    if (tubeFromIndex < 0 || tubeFromIndex >= m_model->tubesCount()
            || tubeToIndex < 0 || tubeToIndex >= m_model->tubesCount())
        return;

    moveColor(m_tubes->at(tubeFromIndex), m_tubes->at(tubeToIndex));
}
