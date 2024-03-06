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
        TubeModel * tm = CtGlobal::game().boardModel()->getTube(i);
        TubeItem * tube = new TubeItem(this, tm);
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
//    QQuickItem::mousePressEvent(event);
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

        m_tubes->at(tubeNumber)->setPivotPoint(
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

void GameBoard::clickTube(TubeItem * tube)
{
    if (tube == nullptr) {
        if (selectedTube != nullptr) {
            qDebug() << "disable selection";
            selectedTube->setSelected(false);
            selectedTube = nullptr;
        }
    }
    else if (selectedTube == nullptr) {
        if (tube->model()->canExtractColor()) {
            qDebug() << "new selection";
            tube->setSelected(true);
            selectedTube = tube;
        }
    }
    else {

        selectedTube->setSelected(false);

        if (tube == selectedTube) {
            qDebug() << "disable selection";
            selectedTube = nullptr;
        } else if (tube->model()->canPutColor(selectedTube->model()->currentColor())) {
            qDebug() << "put color";
            // put color
            selectedTube = nullptr;
        } else {
            qDebug() << "change selection";
            selectedTube = tube;
            tube->setSelected(true);
        }
    }

    int i = 0;
    while (i < m_model->tubesCount()) {
        tube = m_tubes->at(i);

        if (selectedTube != nullptr
                && selectedTube != tube
                && tube->model()->canPutColor(selectedTube->model()->currentColor())                    )
            tube->setShade(1);
        else {
            tube->setShade(0);
        }
        i++;
    }
}

