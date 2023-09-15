#include "colorslayer.h"

#include <QPainter>
#include <QBrush>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/tubeimages.h"
#include "src/palette.h"

ColorsLayer::ColorsLayer(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{

    m_colors = new quint8[4];
    m_colors[0] = 12;
    m_colors[1] = 8;
    m_colors[2] = 1;
    m_colors[3] = 10;
    m_count = 4;

// tilt angles
    m_tiltAngles = new qreal[5];
    m_tiltAngles[0] = 118.6105 / 180 * M_PI;
    m_tiltAngles[1] = 85.252   / 180 * M_PI;
    m_tiltAngles[2] = 77.666   / 180 * M_PI;
    m_tiltAngles[3] = 71.5285  / 180 * M_PI;
    m_tiltAngles[4] = 26.9970  / 180 * M_PI;

// coordinates
    tubeVertices = new PointF[6];
    edgeLines = new QLineF[5];
    tubeSlices = new SliceF[6];
    colorSegments = new SliceF[6];


    m_drawImage = new QImage(280, 200, QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);
    setWidth(280);
    setHeight(200);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    m_fillTimer = new QTimer(this);
    connect(m_fillTimer, &QTimer::timeout, [=](){
        addFillArea(m_fillAreaInc);
        update();
    });

    m_rotateTimer = new QTimer(this);
    connect(m_rotateTimer, &QTimer::timeout, [=](){
//        addFillArea(m_fillAreaInc);

        update();
    });

}

ColorsLayer::~ColorsLayer()
{
    delete [] m_colors;
    delete [] m_tiltAngles;
    delete m_painter;
    delete m_drawImage;
    delete m_fillTimer;
    delete m_rotateTimer;


    delete [] tubeVertices;
    delete [] edgeLines;
    delete [] tubeSlices;
    delete [] colorSegments;

}

qreal ColorsLayer::scale()
{
    return CtGlobal::images().scale();
}

qreal ColorsLayer::angle()
{
    return m_angle;
}

void ColorsLayer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
}

void ColorsLayer::onScaleChanged()
{
    delete m_painter;
    delete m_drawImage;

    m_drawImage = new QImage(280 * scale(), 200 * scale(), QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);

    setWidth(280 * scale());
    setHeight(200 * scale());

    drawColors();
    update();
    emit scaleChanged(scale());
}

void ColorsLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, *m_drawImage);
}

void ColorsLayer::drawColors()
{
    m_drawImage->fill(0x00ffffff);
    if (qFuzzyIsNull(m_angle))
    {
        for (int i = 0; i < m_count; ++i)
        {
            m_colorRect = CtGlobal::images().colorRect(i);
            m_colorRect.translate(100 * scale(), 20 * scale());
            m_painter->fillRect(m_colorRect, CtGlobal::palette().getColor(m_colors[i]));
        }
    }
    else
    {
        m_colorCurrent = 0;
        m_sliceCurrent = 0;
        clearColorSegments();
        m_fillArea = CtGlobal::images().colorArea();

        while (m_sliceCurrent < m_slicesCount-1
               && m_colorCurrent < m_count)
        {
            nextSegment();
            if (qFuzzyIsNull(m_fillArea))
                m_fillArea = CtGlobal::images().colorArea();
        }
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

    path.translate(100 * scale(), 20 * scale());
    m_painter->setBrush(QBrush(CtGlobal::palette().getColor(m_colors[m_colorCurrent])));
    m_painter->setPen(Qt::NoPen);

    m_painter->drawPath(path);

}

void ColorsLayer::fillColors(quint8 colorNum, quint8 count)
{
    if (!qFuzzyIsNull(m_angle) || m_rotateTimer->isActive())
        return;

/*    if (m_count == 4)
    {
        m_count = 0;
        drawColors();
        update();
        return;
    }
*/

    if (m_fillTimer->isActive())
    {
        if (m_colors[m_count - 1] == colorNum)
            m_fillCount += count;
    }
    else
    {
        m_fillArea = 0;
        m_fillAreaInc = CtGlobal::images().colorArea() / 40;
        m_fillColor = CtGlobal::palette().getColor(colorNum);
        m_fillCount = count;
        m_fillJetWidth = 3 * scale();

        m_colorRect = CtGlobal::images().colorRect(m_count);
        m_colorRect.translate(100 * scale(), 20 * scale());
        m_colorBottom = m_colorRect.bottom();

        drawColors();
        update();

        m_fillTimer->start(20);
    }

    for (int i = 0; i < count; i++)
    {
        m_colors[i + m_count] = colorNum;
    }
    m_count += count;
}

void ColorsLayer::addFillArea(qreal fillAreaInc)
{
    m_fillArea += fillAreaInc;

    qreal maxJetHeight = m_colorBottom - 20 * scale();
    qreal maxJetArea = m_fillJetWidth * maxJetHeight;

    QRectF jetRect = QRectF((m_colorRect.right() + m_colorRect.left() - m_fillJetWidth) / 2,
                            20 * scale(), m_fillJetWidth, maxJetHeight);

    qreal fillArea = qMin(m_fillArea, CtGlobal::images().colorArea() * m_fillCount);
    qreal heelArea = m_fillArea - fillArea;
    qreal jetArea = qMin (fillArea, maxJetArea) - heelArea;
    if (jetArea < 0) jetArea = 0.0;
    qreal cupArea = fillArea - jetArea;

    qreal cupHeight = cupArea / CtGlobal::images().colorWidth();
    qreal jetHeight = jetArea / m_fillJetWidth;

    if (!qFuzzyCompare(cupHeight + jetHeight, m_colorBottom))
    {
        if (jetArea > 0)
        {
            if (qFuzzyIsNull(heelArea))
            {   // draws jet rectangle from top
                jetRect.setHeight(jetHeight);
                m_painter->fillRect(jetRect, m_fillColor);
            }
            else
            {   // erases top of the jet rectangle
                jetRect.setHeight(maxJetHeight - jetHeight);
                for (int x = 0; x < jetRect.toRect().width(); x++)
                    for (int y = 0; y < jetRect.toRect().height(); y++)
                        m_drawImage->setPixel(x + jetRect.toRect().x(),
                                              y + jetRect.toRect().y(),
                                              0x00ffffff);
            }
        }
        else m_fillTimer->stop();
    }

    if (cupArea > 0)
    {
        m_colorRect.setTop(m_colorBottom - cupHeight);
        m_painter->fillRect(m_colorRect, m_fillColor);
    }
}


void ColorsLayer::dropColors(quint8 count)
{
}

void ColorsLayer::nextSegment()
{

    if (m_sliceCurrent == 0 && m_colorCurrent == 0)
        m_bottomLine = tubeSlices[0];
    else
        m_bottomLine = m_topLine;

    if (m_segmentsCount == 0)
        addColorSegment(m_bottomLine);

    m_topLine = tubeSlices[m_sliceCurrent+1];

    // size (area) of the segment
    qreal dx0 = m_bottomLine.x2 - m_bottomLine.x1; // bottom section length
    qreal dx1 = m_topLine.x2 - m_topLine.x1;       // top section length
    qreal dy = m_bottomLine.y - m_topLine.y;       // height
    m_sliceArea = dy * (dx0 + dx1) / 2;

    // checks the area
    if (m_fillArea > m_sliceArea)
    {

        // Whole the segment is filled by the current color

        addColorSegment(m_topLine);
        m_fillArea -= m_sliceArea;
        m_sliceArea = 0;
        m_sliceCurrent++;

        if (m_topLine.v == 0) // and is this the last segment?
        {
            drawColorCell();
            clearColorSegments();

//          drawBottle();
//          if (!qFuzzyIsNull(endAngle))
//                drawFlow(colorArea - sliceArea);

            m_sliceArea -= m_fillArea;
            m_fillArea = 0;
            m_colorCurrent ++;
        }

    } else {

        // The current segment is not filled by one color,
        // so we need to calculate size of the current color.

        qreal newHeight = dy;
        if (qFuzzyIsNull(dx0)) {
            // The current segment is a triangle.
            newHeight = 2 * m_fillArea / dx1;

        } else if (qFuzzyIsNull(dx1))  {
            // This is alomst impossible, but let it be.
            // The current segment is a triangle.
            newHeight = 2 * m_fillArea / dx0;

        }  else if (qFuzzyCompare(dx0, dx1)) {
            // The current segment is a parallelogram.
            newHeight = m_fillArea / dx1;

        } else {

            /*
             * The current segment is a trapeze.
             * We have to solve a sqare equation:
             *   ( k/2 ) * x^2 + dx0 * x - S = 0,
             * where k   - a trapeze's coefficient
             *       dx0 - bottom section length
             *       S   - color's area
            */
            qreal k = (dx1 - dx0) / dy;
            qreal D = dx0 * dx0 + 2 * k * m_fillArea;

            if (D < 0) {
                qDebug() << "ERROR! check nextSegment() D value";
                return;
            }

            qreal newH = ( -dx0 + sqrt(D) ) / k;
            if (newH > 0 && newH < dy) {
                newHeight = newH;
            } else {
                newH = ( -dx0 - sqrt(D) ) / k;
                if (newH > 0 && newH < dy) {
                    newHeight = newH;
                } else {
                    qDebug() << "ERROR! check nextSegment() X value";
                    return;
                }
            }
        }

        // Now recalculates top line of the color's segment...
        m_topLine.y = m_bottomLine.y - newHeight;
        m_topLine.x1 = (m_topLine.x1 - m_bottomLine.x1) * newHeight / dy + m_bottomLine.x1;
        m_topLine.x2 = (m_topLine.x2 - m_bottomLine.x2) * newHeight / dy + m_bottomLine.x2;

        // and draws the color
        addColorSegment(m_topLine);
        drawColorCell();
        clearColorSegments();
        m_sliceArea -= m_fillArea;
        m_fillArea = 0;
        m_colorCurrent ++;
    }

}

void ColorsLayer::setAngle(qreal newAngle)
{

    if (qFuzzyCompare(m_angle, newAngle))
        return;

    m_angle = newAngle;

    if (qFuzzyIsNull(m_angle))
    {
        drawColors();
        update();
        return;
    }

// ---- set rotation point
    quint8 rVertexNumber; // rotation vertex number
    if (m_angle > 0)
        rVertexNumber = 0;
    else
        rVertexNumber = 5;

    tubeVertices[0].v = 0;
    tubeVertices[0].x = CtGlobal::images().vertex(rVertexNumber).x();
    tubeVertices[0].y = CtGlobal::images().vertex(rVertexNumber).y();

// --- calculate polar coordinates of other vertices & rotate them
    for (quint8 i = 0; i < 6; i++)
    {
        if (i != rVertexNumber) {
            quint8 number = abs(rVertexNumber - i);

            qreal dx = CtGlobal::images().vertex(i).x() - tubeVertices[0].x;
            qreal dy = CtGlobal::images().vertex(i).y() - tubeVertices[0].y;
            qreal radius = sqrt(dx*dx + dy*dy);
            qreal angle = atan2(dy, dx) + m_angle;

            tubeVertices[number].v = number;
            tubeVertices[number].x = tubeVertices[0].x + radius * qCos(angle);
            tubeVertices[number].y = tubeVertices[0].y + radius * qSin(angle);
        }
    }

// --- calculate edges before sorting points
    qDebug() << "edges:";
    for (qint8 i = 0; i < 5; i++)
    {
        edgeLines[i] = QLineF(tubeVertices[i].x, tubeVertices[i].y,
                              tubeVertices[i+1].x, tubeVertices[i+1].y);
    }

// --- sort points by vertical
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

    qDebug() << "after points sort:";
    for (int i = 0; i < 6; ++i) {
        qDebug() << "point:" << i
                 << "p_num:" << tubeVertices[i].v
                 << "x:"     << tubeVertices[i].x
                 << "y:"     << tubeVertices[i].y;
    }

// --- calculate slices
    clearSlices();
    if (tubeVertices[0].v != 0)             // rotation point must be not a lowest point
    {
        quint8 currentPoint;

        // lowest point(s)
        if (qFuzzyCompare(qAbs(m_angle), (qreal)M_PI_2))
        {   // if angle = +-90 deg, we have two lowest points
            addSlice(tubeVertices[1].v, tubeVertices[0].x,
                    tubeVertices[1].x, tubeVertices[1].y);
            currentPoint = 1;
            qDebug() << "lowest:" << tubeVertices[0].v << "-" << tubeVertices[1].v;
        }
        else
        {
            addSlice(tubeVertices[0].v, tubeVertices[0].x,
                    tubeVertices[0].x, tubeVertices[0].y);
            currentPoint = 0;
            qDebug() << "lowest:" << tubeVertices[0].v;
        }

        // other points
        do {
            currentPoint++;
            addSlice(tubeVertices[currentPoint].v,
                    getIntersection(currentPoint),
                    tubeVertices[currentPoint].x,
                    tubeVertices[currentPoint].y);
        } while (tubeVertices[currentPoint].v != 0);
    }

    qDebug() << "slices:" << m_slicesCount;
    for (int i = 0; i < m_slicesCount; ++i) {
        qDebug() << "slice:" << i
                 << "p_num:" << tubeSlices[i].v
                 << "x1:"    << tubeSlices[i].x1
                 << "x2:"    << tubeSlices[i].x2
                 << "y:"     << tubeSlices[i].y;
    }

    emit angleChanged(m_angle);

    drawColors();
    update();
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

qreal ColorsLayer::getIntersection(quint8 vertex)
{
    qint8 line = 0;
    while (line < 5)
    {
        if ((line != tubeVertices[vertex].v)
            && (line + 1 != tubeVertices[vertex].v))
        {
            qreal minY = qMin(edgeLines[line].p1().y(), edgeLines[line].p2().y());
            qreal maxY = qMax(edgeLines[line].p1().y(), edgeLines[line].p2().y());

            if (tubeVertices[vertex].y >= minY && tubeVertices[vertex].y <= maxY)
            {
                return (tubeVertices[vertex].y - edgeLines[line].p1().y())
                        * edgeLines[line].dx() / edgeLines[line].dy()
                        + edgeLines[line].p1().x();
            }
        }
        line++;
    }
    return -1000;
}


void ColorsLayer::addColorSegment(SliceF line)
{
    addColorSegment(line.x1, line.x2, line.y);
}


void ColorsLayer::addColorSegment(qreal x1, qreal x2, qreal y)
{
    qDebug() << "color No" << m_colorCurrent
             << "segment No" << m_segmentsCount
             << "x1:"    << qMin(x1, x2)
             << "x2:"    << qMax(x1, x2)
             << "y:"     << y;


    colorSegments[m_segmentsCount].x1 = qMin(x1, x2);
    colorSegments[m_segmentsCount].x2 = qMax(x1, x2);
    colorSegments[m_segmentsCount].y = y;
    m_segmentsCount++;
}

void ColorsLayer::clearColorSegments()
{
    for (int i = 0; i < 6; i++)
        colorSegments[i] = {};
    m_segmentsCount = 0;
}


