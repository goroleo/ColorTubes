#include "colorslayer.h"

#include "src/ctglobal.h"
#include "src/game.h"
#include "src/tubeimages.h"
#include "src/palette.h"
#include "core/tubemodel.h"
#include "gui/tubeitem.h"

ColorsLayer::ColorsLayer(TubeItem * parent) :
    QQuickPaintedItem((QQuickItem *) parent)
{
    m_tube = parent;

// coordinates
    tubeVertices = new PointF[6];
    bottleLines = new LineF[5];
    tubeSlices = new SliceF[6];
    colorSegments = new SliceF[6];

    m_drawImage = new QImage(280 * CtGlobal::images().scale(),
                             200 * CtGlobal::images().scale(),
                             QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    setWidth(280 * scale());
    setHeight(200 * scale());

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    QObject::connect(parent, &TubeItem::angleChanged,
            this, &ColorsLayer::onAngleChanged);

    m_fillTimer = new QTimer(this);
    connect(m_fillTimer, &QTimer::timeout, [=]() {
//        addFillArea(m_fillAreaInc);
        update();
    });

    if (m_tube->model())
        refresh();
}

ColorsLayer::~ColorsLayer()
{
    delete m_painter;
    delete m_drawImage;
    delete m_fillTimer;

    delete [] tubeVertices;
    delete [] bottleLines;
    delete [] tubeSlices;
    delete [] colorSegments;
}

qreal ColorsLayer::scale()
{
    return CtGlobal::images().scale();
}

TubeModel * ColorsLayer::model()
{
    return m_tube->model();
}

void ColorsLayer::onScaleChanged()
{

    if (m_drawImage) {
        delete m_painter;
        delete m_drawImage;
    }

    m_drawImage = new QImage(280 * scale(), 200 * scale(), QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    setWidth(280 * scale());
    setHeight(200 * scale());

    drawColors();
    update();
}

void ColorsLayer::onAngleChanged()
{

    qreal angle = m_tube->angle();

    if (qFuzzyIsNull(angle))
    {
        m_tube->setVerticalShift(0);
        drawColors();
        update();
        return;
    }

// ---- set rotation point
    quint8 rVertexNumber; // rotation vertex number
    if (angle > 0)
        rVertexNumber = 0;
    else
        rVertexNumber = 5;

    tubeVertices[0].v = 0;
    tubeVertices[0].x = CtGlobal::images().vertex(rVertexNumber).x();
    tubeVertices[0].y = CtGlobal::images().vertex(rVertexNumber).y();

// --- calculate new coordinates of other vertices after rotation
    qreal cos = qCos(angle);
    qreal sin = qSin(angle);

    for (quint8 i = 0; i < 6; i++)
    {
        if (i != rVertexNumber) {
            quint8 number = abs(rVertexNumber - i);

            qreal dx = CtGlobal::images().vertex(i).x() - tubeVertices[0].x;
            qreal dy = CtGlobal::images().vertex(i).y() - tubeVertices[0].y;

            tubeVertices[number].v = number;
            tubeVertices[number].x = tubeVertices[0].x + dx * cos - dy * sin;
            tubeVertices[number].y = tubeVertices[0].y + dx * sin + dy * cos;
        }
    }

// --- calculate edge lines before sorting points
    for (qint8 i = 0; i < 5; i++)
    {
        bottleLines[i].x1 = tubeVertices[i].x;
        bottleLines[i].y1 = tubeVertices[i].y;
        bottleLines[i].x2 = tubeVertices[i+1].x;
        bottleLines[i].y2 = tubeVertices[i+1].y;

        bottleLines[i].k = (bottleLines[i].y2 - bottleLines[i].y1)
                / (bottleLines[i].x2 - bottleLines[i].x1);
        bottleLines[i].b = bottleLines[i].y1 - (bottleLines[i].k * bottleLines[i].x1);
    }

// --- sort points by vertical. The lowest point will be tubeVertices[0]
    qint8 j;
    PointF temp;
    for (qint8 i = 1; i < 6; ++i) {
        temp = tubeVertices[i];
        j = i;
        while ( (j > 0) && (tubeVertices[j-1].y < temp.y) ) {
            tubeVertices[j] = tubeVertices[j-1];
            j--;
        }
        tubeVertices[j] = temp;
    }

    m_tube->setVerticalShift(tubeVertices[5].y);

// --- calculate slices
    clearSlices();
    if (tubeVertices[0].v != 0)             // rotation point must be not the lowest point
    {
        quint8 currentVertex;

        // lowest point(s)
        if ( qFuzzyCompare(qAbs(angle), (qreal) CT_PI2) )
        {
            // when angle = +-90 degrees, we have two lowest points
            addSlice(tubeVertices[1].v, tubeVertices[0].x,
                     tubeVertices[1].x, tubeVertices[1].y);
            currentVertex = 1;
        }
        else
        {
            addSlice(tubeVertices[0].v, tubeVertices[0].x,
                     tubeVertices[0].x, tubeVertices[0].y);
            currentVertex = 0;
        }

        // other points
        do {
            currentVertex++;
            addSlice(tubeVertices[currentVertex].v,
                     tubeVertices[currentVertex].x,
                     getIntersectionX(currentVertex),
                     tubeVertices[currentVertex].y);
        } while (tubeVertices[currentVertex].v != 0);
    }

    drawColors();
    update();
}

void ColorsLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, * m_drawImage);
}

void ColorsLayer::drawColors()
{

    m_drawImage->fill(0x30ff8888);
    if (!model())
        return;

    if (qFuzzyIsNull(m_tube->angle()))
    {
        for (quint8 i = 0; i < model()->count(); ++i)
        {
            m_colorRect = CtGlobal::images().colorRect(i);
            m_colorRect.translate(0, 20 * scale());
            m_painter->fillRect(m_colorRect, CtGlobal::palette().getColor(model()->color(i)));
        }
    }
    else
    {
        m_colorCurrent = 0;
        m_sliceCurrent = 0;

        clearColorSegments();

        m_fillArea = CtGlobal::images().colorArea();

        while (m_sliceCurrent < m_slicesCount-1
               && m_colorCurrent < model()->count())
        {
            nextSegment();
            if (qFuzzyIsNull(m_fillArea))
                m_fillArea = CtGlobal::images().colorArea();
        }
    }
}

void ColorsLayer::nextSegment()
{

    if (m_sliceCurrent == 0 && m_colorCurrent == 0)
        m_bottomLine = tubeSlices[0];
    else
        m_bottomLine = m_topLine;

    if (m_segmentsCount == 0)
        addColorSegment(m_bottomLine);

    m_topLine = tubeSlices[m_sliceCurrent + 1];

    // size (area) of the segment
    qreal dx0 = m_bottomLine.x2 - m_bottomLine.x1; // bottom section length
    qreal dx1 = m_topLine.x2 - m_topLine.x1;       // top section length
    qreal dy = m_bottomLine.y - m_topLine.y;       // height
    qreal sliceArea = dy * (dx0 + dx1) / 2;

    // checks the area
    if (m_fillArea > sliceArea)
    {

        // Whole the segment is filled by the current color

        addColorSegment(m_topLine);
        m_fillArea -= sliceArea;
        m_sliceCurrent++;

        if (m_topLine.v == 0) // is this the last segment?
        {
            drawColorCell();
            clearColorSegments();

//          if (!qFuzzyIsNull(endAngle))
//                drawFlow(colorArea - sliceArea);

            m_fillArea = 0;
            m_colorCurrent ++;
        }

    } else {

        // The current segment is not filled by one color,
        // so we need to calculate size of the current color.

        qreal newHeight;
        if (qFuzzyIsNull(dx1))  {
            // This is almost impossible, but let it be.
            // The current segment is a triangle.
            newHeight = 2 * m_fillArea / dx0;

        }  else if (qFuzzyCompare(dx0, dx1)) {
            // The current segment is a parallelogram.
            newHeight = m_fillArea / dx1;

        } else {

            /*
             * The current segment is a trapeze or a triangle.
             * We have to solve a sqare equation:
             *   ( k/2 ) * x^2 + dx0 * x - S = 0,
             * where k   - a trapeze's coefficient
             *       dx0 - bottom section length
             *       S   - color's fill area
            */
            qreal k = (dx1 - dx0) / dy;
            qreal D = dx0 * dx0 + 2 * k * m_fillArea;

            newHeight = ( -dx0 + sqrt(D) ) / k;
            if (!(newHeight > 0 && newHeight < dy)) {
                newHeight = ( -dx0 - sqrt(D) ) / k;
            }
        }

        // now recalculates top line...
        m_topLine.y = m_bottomLine.y - newHeight;
        m_topLine.x1 = (m_topLine.x1 - m_bottomLine.x1) * newHeight / dy + m_bottomLine.x1;
        m_topLine.x2 = (m_topLine.x2 - m_bottomLine.x2) * newHeight / dy + m_bottomLine.x2;

        // ...and draws the color
        addColorSegment(m_topLine);
        drawColorCell();
        clearColorSegments();
        m_fillArea = 0;
        m_colorCurrent ++;
    }
}

void ColorsLayer::drawColorCell()
{
    QPainterPath path;
    path.moveTo(colorSegments[0].x1, colorSegments[0].y);
    int i = 1;
    do {
        path.lineTo(colorSegments[i].x1, colorSegments[i].y);
        i++;
    } while (i != m_segmentsCount);
    do {
        i--;
        path.lineTo(colorSegments[i].x2, colorSegments[i].y);
    } while (i != 0);
    path.lineTo(colorSegments[0].x1, colorSegments[0].y);

    path.translate(100 * scale(), 20 * scale() - m_tube->verticalShift());
    m_painter->setBrush(QBrush(CtGlobal::palette().getColor(model()->color(m_colorCurrent))));
    m_painter->setPen(Qt::NoPen);
    m_painter->drawPath(path);
}

void ColorsLayer::addSlice(qint8 vertex, qreal x1, qreal x2, qreal y)
{
    tubeSlices[m_slicesCount].v = vertex;
    tubeSlices[m_slicesCount].x1 = qMin(x1, x2);
    tubeSlices[m_slicesCount].x2 = qMax(x1, x2);
    tubeSlices[m_slicesCount].y = y;
    m_slicesCount++;
}

void ColorsLayer::clearSlices()
{
    for (int i = 0; i < 6; i++)
        tubeSlices[i] = {};
    m_slicesCount = 0;
}

void ColorsLayer::addColorSegment(SliceF line)
{
    colorSegments[m_segmentsCount].x1 = line.x1;
    colorSegments[m_segmentsCount].x2 = line.x2;
    colorSegments[m_segmentsCount].y = line.y;
    m_segmentsCount++;
}

void ColorsLayer::clearColorSegments()
{
    for (int i = 0; i < 6; i++)
        colorSegments[i] = {};
    m_segmentsCount = 0;
}

qreal ColorsLayer::getIntersectionX(quint8 vertex)
{
    qint8 line = 0;
    while (line < 5)
    {
        if ((tubeVertices[vertex].v != line)
            && (tubeVertices[vertex].v) != line + 1)
        {
            qreal x = (tubeVertices[vertex].y - bottleLines[line].b) / bottleLines[line].k;

            if (x >= qMin(bottleLines[line].x1, bottleLines[line].x2)
                    && x <= qMax(bottleLines[line].x1, bottleLines[line].x2))
            {
                return x;
            }
        }
        line++;
    }
    return -1000;
}

void ColorsLayer::refresh()
{
    drawColors();
    update();
}
