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

void GameBoard::reScale()
{
    qreal newScale = 0.0;

    int cols = m_model->tubesCount() / 2;
    if (cols * 2 < m_model->tubesCount())
        cols ++;

    int tubesWidth = cols * (80 + spaceX) + spaceX;
    int tubesHeight = 400 + 3 * spaceY;

    newScale = qMin(qreal(width() / tubesWidth), qreal(height() / tubesHeight));

    qDebug() << "rescale" << m_model->tubesCount() << cols << "newScale" << newScale << "imgScale" << scale();
    qDebug() << "width" << width() << "height" << height();

    if (!qFuzzyIsNull(newScale) && !qFuzzyCompare(scale(), newScale))
    {
        qDebug() << "newScale" << newScale;
        CtGlobal::images().setScale(newScale);
    } else {
        placeTubes();
    }
}

void GameBoard::placeTubes()
{
    TubeItem * tube;

    int cols = m_model->tubesCount() / 2;
    if (cols * 2 < m_model->tubesCount())
        cols ++;

    qreal leftMargin = scale() * spaceX;
    qreal rowWidth = width() - leftMargin * 2;
    qreal tubeWidth = scale() * 80;
    int rowColumns = cols;
    qreal rowSpace = (rowWidth - rowColumns * tubeWidth) / (rowColumns + 1);

    qreal realHeight = scale() * (400 + 3 * spaceY);
    qreal topMargin = qreal (height() - realHeight) / 2;

    qDebug() << "height" << height()
             << "realHeight" << realHeight
             << "top" << topMargin
             << "total" << topMargin * 2 + realHeight;

    int row = 0;
    int col = 0;
    int tubeNumber = 0;

    while (tubeNumber < m_model->tubesCount()) {

        if (tubeNumber == cols) {
            row = 1;
            rowColumns = m_model->tubesCount() - cols;
            rowSpace = (rowWidth - rowColumns * tubeWidth) / (rowColumns + 1);
        }

        col = tubeNumber - row * cols;

        tube = m_tubes->at(tubeNumber);

        tube->setPivotPoint(
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

