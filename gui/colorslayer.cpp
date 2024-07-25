#include "colorslayer.h"

#include "src/ctglobal.h"
#include "src/game.h"
#include "src/ctimages.h"
#include "src/ctpalette.h"
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

    m_drawImage = new QImage(CtGlobal::images().tubeFullWidth(),
                             CtGlobal::images().tubeFullHeight(),
                             QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    setWidth(CtGlobal::images().tubeFullWidth());
    setHeight(CtGlobal::images().tubeFullHeight());

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    QObject::connect(parent, &TubeItem::angleChanged,
            this, &ColorsLayer::onAngleChanged);

    if (m_tube->model())
        refresh();
}

ColorsLayer::~ColorsLayer()
{
    if (m_drawImage) {
        delete m_painter;
        delete m_drawImage;
    }
    delete [] tubeVertices;
    delete [] bottleLines;
    delete [] tubeSlices;
    delete [] colorSegments;
}

void ColorsLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, * m_drawImage);
}

void ColorsLayer::refresh()
{
    drawColors();
    update();
}

qreal ColorsLayer::scale()
{
    return CtGlobal::images().scale();
}

void ColorsLayer::onScaleChanged()
{
    if (m_drawImage) {
        delete m_painter;
        delete m_drawImage;
    }

    m_drawImage = new QImage(
                CtGlobal::images().tubeFullWidth(),
                CtGlobal::images().tubeFullHeight(),
                QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    setWidth(CtGlobal::images().tubeFullWidth());
    setHeight(CtGlobal::images().tubeFullHeight());

    drawColors();
    update();
}

void ColorsLayer::onAngleChanged()
{
    qreal angle = m_tube->angle();

    if (qFuzzyIsNull(angle)) {
        m_tube->setVerticalShift(0);
        drawColors();
        update();
        return;
    }

// ---- set rotation point
    quint8 rotationVertex; // rotation vertex number
    if (angle > 0)
        rotationVertex = 0;
    else
        rotationVertex = 5;

    tubeVertices[0].v = 0;
    tubeVertices[0].x = CtGlobal::images().vertex(rotationVertex).x();
    tubeVertices[0].y = CtGlobal::images().vertex(rotationVertex).y();

// --- calculate new coordinates of other vertices after rotation
    qreal cos = qCos(angle);
    qreal sin = qSin(angle);

    for (quint8 i = 0; i < 6; i++)
    {
        if (i != rotationVertex) {
            quint8 number = abs(rotationVertex - i);

            qreal dx = CtGlobal::images().vertex(i).x() - tubeVertices[0].x;
            qreal dy = CtGlobal::images().vertex(i).y() - tubeVertices[0].y;

            tubeVertices[number].v = number;
            tubeVertices[number].x = tubeVertices[0].x + dx * cos - dy * sin;
            tubeVertices[number].y = tubeVertices[0].y + dx * sin + dy * cos;
        }
    }

// --- calculate edge lines before sort vertices
    for (qint8 i = 0; i < 5; i++)
    {
        bottleLines[i].x1 = tubeVertices[i].x;
        bottleLines[i].y1 = tubeVertices[i].y;
        bottleLines[i].x2 = tubeVertices[i+1].x;
        bottleLines[i].y2 = tubeVertices[i+1].y;

        if (qFuzzyCompare(bottleLines[i].x1, bottleLines[i].x2)) {
            // avoid division by zero
            bottleLines[i].k = CtGlobal::images().tubeFullWidth();
            bottleLines[i].b = CtGlobal::images().tubeFullHeight();
        } else {
            bottleLines[i].k = (bottleLines[i].y2 - bottleLines[i].y1)
                    / (bottleLines[i].x2 - bottleLines[i].x1);
            bottleLines[i].b = bottleLines[i].y1 - (bottleLines[i].k * bottleLines[i].x1);
        }
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

    // Parent tube has to be visible. This is to avoid negative vertical coordinates after rotation.
    m_tube->setVerticalShift(tubeVertices[5].y);

// --- calculate slices
    clearSlices();
    if (tubeVertices[0].v != 0)              // rotation point must be not the lowest point
    {
        quint8 currentVertex;

        // lowest point(s)
        if ( qFuzzyCompare(qAbs(angle), CT_PI2) ) {
            // when angle = +-90 degrees, we have two lowest points
            addSlice(tubeVertices[1].v, tubeVertices[0].x,
                     tubeVertices[1].x, tubeVertices[1].y);
            currentVertex = 1;
        } else {
            addSlice(tubeVertices[0].v, tubeVertices[0].x,
                     tubeVertices[0].x, tubeVertices[0].y);
            currentVertex = 0;
        }

        // other points
        do {
            currentVertex++;
            addSlice(tubeVertices[currentVertex].v,
                     tubeVertices[currentVertex].x,
                     intersectByX(currentVertex),
                     tubeVertices[currentVertex].y);
        } while (tubeVertices[currentVertex].v != 0);
    }

    drawColors();
    update();
}

void ColorsLayer::drawColors()
{
    if (!m_tube->model())
        return;

    m_drawImage->fill(0);

    if (qFuzzyIsNull(m_tube->angle())) {

        for (quint8 i = 0; i < m_tube->model()->count(); ++i)
        {
            m_colorRect = CtGlobal::images().colorRect(i);
            m_colorRect.translate(0, CtGlobal::images().shiftHeight());
            m_painter->fillRect(m_colorRect, CtGlobal::paletteColor(m_tube->colorAt(i)));
        }

        m_bottomLine.y = CtGlobal::images().vertex(3).y()
                - CtGlobal::images().colorHeight() * m_tube->model()->count()
                + CtGlobal::images().shiftHeight();
        m_bottomLine.x1 = CtGlobal::images().vertex(3).x();

    } else {
        clearColorSegments();
        m_colorIndex = 0;
        m_sliceIndex = 0;
        m_fillArea = CtGlobal::images().colorArea();

        while (m_sliceIndex < m_slicesCount - 1
               && m_colorIndex < m_tube->model()->count())
        {
            nextSegment();
            if (qFuzzyIsNull(m_fillArea))
                m_fillArea = CtGlobal::images().colorArea();
        }
    }
}

void ColorsLayer::nextSegment()
{

    if (m_sliceIndex == 0 && m_colorIndex == 0)
        m_bottomLine = tubeSlices[0];
    else
        m_bottomLine = m_topLine;

    if (m_segmentsCount == 0)
        addColorSegment(m_bottomLine);

    m_topLine = tubeSlices[m_sliceIndex + 1];

    // size (area) of the segment
    qreal dx0 = m_bottomLine.x2 - m_bottomLine.x1; // bottom section length
    qreal dx1 = m_topLine.x2 - m_topLine.x1;       // top section length
    qreal dy = m_bottomLine.y - m_topLine.y;       // height
    qreal sliceArea = dy * (dx0 + dx1) / 2;

    // checks the area
    if (m_fillArea > sliceArea)
    {

        // whole the segment is filled by the current color

        addColorSegment(m_topLine);
        m_fillArea -= sliceArea;
        m_sliceIndex++;

        if (m_topLine.v == 0) // is this the last segment?
        {
            drawColorCell();
            clearColorSegments();
            m_colorIndex ++;
        }

    } else {

        // The current segment is not filled by one color,
        // so we need to calculate size of the current color.

        qreal newHeight;
        if (qFuzzyIsNull(dx1))  {
            // This is impossible, but let it be.
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
        m_colorIndex ++;
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

    path.translate(CtGlobal::images().shiftWidth(),
                   CtGlobal::images().shiftHeight() - m_tube->m_verticalShift);
    m_painter->setBrush(QBrush(CtGlobal::paletteColor(m_tube->colorAt(m_colorIndex))));
    m_painter->drawPath(path);
}

void ColorsLayer::clearSlices()
{
    for (int i = 0; i < 6; i++)
        tubeSlices[i] = {};
    m_slicesCount = 0;
}

void ColorsLayer::addSlice(qint8 vertex, qreal x1, qreal x2, qreal y)
{
    tubeSlices[m_slicesCount].v = vertex;
    tubeSlices[m_slicesCount].x1 = qMin(x1, x2);
    tubeSlices[m_slicesCount].x2 = qMax(x1, x2);
    tubeSlices[m_slicesCount].y = y;
    m_slicesCount++;
}

void ColorsLayer::clearColorSegments()
{
    for (int i = 0; i < 6; i++)
        colorSegments[i] = {};
    m_segmentsCount = 0;
}

void ColorsLayer::addColorSegment(SliceF line)
{
    colorSegments[m_segmentsCount].x1 = line.x1;
    colorSegments[m_segmentsCount].x2 = line.x2;
    colorSegments[m_segmentsCount].y = line.y;
    m_segmentsCount++;
}

qreal ColorsLayer::intersectByX(quint8 vertex)
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

void ColorsLayer::addPourArea(qreal areaIncrement, quint8 colorIndex)
{
    addPourArea(areaIncrement, m_bottomLine.y * CtGlobal::images().jetWidth(), colorIndex);
}

void ColorsLayer::addPourArea(qreal areaIncrement, qreal jetArea, quint8 colorIndex)
{
    m_pourArea += areaIncrement;
    QColor color = CtGlobal::paletteColor(colorIndex);
    qreal colorHeight;

    if (jetArea > 0) {
        m_colorRect = CtGlobal::images().jetRect();
        if (m_pourArea < jetArea) {
            m_colorRect.setHeight(m_pourArea / m_colorRect.width());
            m_painter->fillRect(m_colorRect, color);
            colorHeight = 0.0;
        } else {
            m_colorRect.setHeight(m_bottomLine.y);
            m_painter->fillRect(m_colorRect, color);
            colorHeight = (m_pourArea - jetArea)
                    / (CtGlobal::images().colorWidth() - CtGlobal::images().jetWidth());
        }
    } else {
        colorHeight = m_pourArea / CtGlobal::images().colorWidth();
        // erasing jet rect by transparent color
        for (int y = 0; y < ceil(m_bottomLine.y - colorHeight); ++y ) {
            for (int x = floor(CtGlobal::images().jetRect().left());
                     x < ceil(CtGlobal::images().jetRect().right()); ++x )
                m_drawImage->setPixel(x, y, 0);
        }
    }

    if (colorHeight > 0) {
        m_colorRect.setRect(m_bottomLine.x1,
                            m_bottomLine.y - colorHeight,
                            CtGlobal::images().colorWidth(),
                            colorHeight);
        m_painter->fillRect(m_colorRect, color);
    }
    update();
}
