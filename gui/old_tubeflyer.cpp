#include "old_tubeflyer.h"

#include "src/ctglobal.h"
#include "src/tubeimages.h"
#include "core/tubemodel.h"
#include "src/palette.h"

#include <QtMath>
#include <QDebug>


old_TubeFlyer::old_TubeFlyer(QQuickItem *parent) :
      QQuickPaintedItem(parent)
{
    bottleCoordinates = new PointF[6];
    bottlePolarCoordinates = new PointP[6];
    slicesCoordinates = new SliceCoo[6];
    segmentsCoordinates = new SegmentCoo[MAX_SEGMENT_POINTS];
    edgeLines = new QLineF[6];
    tiltAngles = new qreal[6];

    tiltAngles[0] = 118.6105 / 180 * M_PI;
    tiltAngles[1] = 85.252 / 180 * M_PI;
    tiltAngles[2] = 77.666 / 180 * M_PI;
    tiltAngles[3] = 71.5285 / 180 * M_PI;
    tiltAngles[4] = 26.9970 / 180 * M_PI;

    setScale(CtGlobal::images().scale());
    setRotation(0);

    internalTimer = new QTimer(this);
    connect(internalTimer, &QTimer::timeout, [=](){
        rotateBottlePoints();
        drawColors();
        update();
    });
}

old_TubeFlyer::~old_TubeFlyer()
{
    delete bottleCoordinates;
    delete bottlePolarCoordinates;
    delete slicesCoordinates;
    delete segmentsCoordinates;
    delete edgeLines;
    delete tiltAngles;
}

void old_TubeFlyer::setTube(TubeModel &tm)
{
    m_tube = &tm;
}

qreal old_TubeFlyer::scale()
{
    return CtGlobal::images().scale();
}


void old_TubeFlyer::setScale(qreal newScale)
{
    CtGlobal::images().setScale(newScale);
    m_drawImage = QImage(280 * newScale,
                         200 * newScale,
                         QImage::Format_ARGB32);
//    update();
    emit scaleChanged(CtGlobal::images().scale());
}



void old_TubeFlyer::setRotationPoint(int number)
{
    // number can be 0 or 5
    rotationPoint.x = CtGlobal::images().vertex(number).x();
    rotationPoint.y = CtGlobal::images().vertex(number).y();
    rotationPoint.pointNumber = number;
    bottleCoordinates[0].pointNumber = 0;
    bottleCoordinates[0].x = rotationPoint.x;
    bottleCoordinates[0].y = rotationPoint.y;

    for (int i = 0; i < 6; i++)
    {
        double dx = CtGlobal::images().vertex(i).x() - rotationPoint.x;
        double dy = CtGlobal::images().vertex(i).y() - rotationPoint.y;
        bottlePolarCoordinates[abs(number-i)].r = sqrt(dx*dx + dy*dy);
        bottlePolarCoordinates[abs(number-i)].a = atan2(dy, dx);
    }
}


void old_TubeFlyer::rotateBottlePoints()
{
    // calculate points
    for (qint8 i = 1; i < 6; i++)
    {
        bottleCoordinates[i].x = rotationPoint.x + bottlePolarCoordinates[i].r
                * qCos(bottlePolarCoordinates[i].a + angle);
        bottleCoordinates[i].y = rotationPoint.y + bottlePolarCoordinates[i].r
                * qSin(bottlePolarCoordinates[i].a + angle);
    }

    // set edges before sorting points
    for (qint8 i = 0; i < 4; i++)
    {
        edgeLines[i].p1() = QPointF(bottleCoordinates[i].x, bottleCoordinates[i].y);
        edgeLines[i].p2() = QPointF(bottleCoordinates[i+1].x, bottleCoordinates[i+1].y);
    }

    // sort points by vertical coordinates
    int j;
    PointF temp;
    for (int i = 1; i < 6; ++i) {
        temp.x = bottleCoordinates[i].x;
        temp.y = bottleCoordinates[i].y;
        temp.pointNumber = bottleCoordinates[i].pointNumber;
        j = i;

        while ( (j > 0) && (bottleCoordinates[j-1].y < temp.y) ) {
            bottleCoordinates[j].x = bottleCoordinates[j-1].x;
            bottleCoordinates[j].y = bottleCoordinates[j-1].y;
            bottleCoordinates[j].pointNumber = bottleCoordinates[j-1].pointNumber;
            j--;
        }
        bottleCoordinates[j].x = temp.x;
        bottleCoordinates[j].y = temp.y;
        bottleCoordinates[j].pointNumber = temp.pointNumber;
    }

    // set slices
    clearSlicesPoints();
    if (bottleCoordinates[0].pointNumber != 0           // rotation point is not a lowest point
            && !qFuzzyIsNull(angle)                     // angle != 0 deg
            && !qFuzzyCompare(qAbs(angle), M_PI/2))     // angle != +-90 deg
    {

        // add lowest point
        addSlicePoints(bottleCoordinates[0].pointNumber, bottleCoordinates[0].x, bottleCoordinates[0].x, bottleCoordinates[0].y);

        quint8 currentPoint = 0;
        do {
            currentPoint++;
            qreal x = getIntersection(currentPoint);
            addSlicePoints(bottleCoordinates[currentPoint].pointNumber,
                     qMax(x, bottleCoordinates[currentPoint].x),
                     qMin(x, bottleCoordinates[currentPoint].x),
                     bottleCoordinates[currentPoint].y);
        } while (bottleCoordinates[currentPoint].pointNumber != rotationPoint.pointNumber);
    }
}

qreal old_TubeFlyer::getIntersection(quint8 point)
{

    QLineF horizLine = QLineF(0,         bottleCoordinates[point].y,
                              m_width-1, bottleCoordinates[point].y);
    QPointF i_point;

    qint8 line = 0;
    while (line < 4)
    {
        if ((line != bottleCoordinates[point].pointNumber)
            && (line + 1 != bottleCoordinates[point].pointNumber))
        {
            if (horizLine.intersect(edgeLines[line], &i_point) == QLineF::BoundedIntersection)
                return i_point.x();
        }
        line++;
    }
    return -1000;
}


void old_TubeFlyer::addSlicePoints(qint8 pNumber, qreal x1, qreal x2, qreal y)
{
    slicesCoordinates[slicesCount].pointNumber = pNumber;
    slicesCoordinates[slicesCount].x1 = x1;
    slicesCoordinates[slicesCount].x2 = x2;
    slicesCoordinates[slicesCount].y = y;
    slicesCount++;
}

void old_TubeFlyer::clearSlicesPoints()
{
    for (int i = 0; i < 6; i++)
    {
        slicesCoordinates[segmentsCount].pointNumber = 0;
        slicesCoordinates[segmentsCount].x1 = 0;
        slicesCoordinates[segmentsCount].x2 = 0;
        slicesCoordinates[segmentsCount].y = 0;
    }
    slicesCount = 0;
}

void old_TubeFlyer::addColorPoints(qreal x1, qreal x2, qreal y)
{
    segmentsCoordinates[segmentsCount].x1 = x1;
    segmentsCoordinates[segmentsCount].x2 = x2;
    segmentsCoordinates[segmentsCount].y = y;
    segmentsCount++;
}

void old_TubeFlyer::clearColorsPoints()
{
    for (int i = 0; i < MAX_SEGMENT_POINTS; i++)
    {
        segmentsCoordinates[segmentsCount].x1 = 0;
        segmentsCoordinates[segmentsCount].x2 = 0;
        segmentsCoordinates[segmentsCount].y = 0;
    }
    segmentsCount = 0;
}


void old_TubeFlyer::drawColors()
{
    bottleDrawn = false;
    m_drawImage.fill(0x00ffffff);
//    grColors.clearRect(0, 0, 300, 300);
//    grBottle.clearRect(0, 0, 300, 300);

    if (qFuzzyIsNull(angle))         // angle = 0 deg;
    {
        for (int i = 0; i < m_tube->count(); ++i) {
            clearColorsPoints();
            colorCurrent = i;
            m_bottomLine.y = bottleCoordinates[2].y - (CtGlobal::images().colorHeight() * i);
            m_topLine.y = m_bottomLine.y - CtGlobal::images().colorHeight();
            addColorPoints(bottleCoordinates[3].x, bottleCoordinates[2].x, m_bottomLine.y);
            addColorPoints(bottleCoordinates[3].x, bottleCoordinates[2].x, m_topLine.y);
            drawColorCell();
        }
        vTranslate = 0;
    }
    else if (qFuzzyCompare(qAbs(angle), M_PI/2)) // angle = +-90 deg;
    {
        // only lowest color can be drawn.
        clearColorsPoints();
        colorCurrent = 0;
        addColorPoints(bottleCoordinates[2].x, bottleCoordinates[1].x, slicesCoordinates[0].y);
        addColorPoints(bottleCoordinates[2].x, slicesCoordinates[1].x1, slicesCoordinates[1].y);
        drawColorCell();
    }
    else // any other angle
    {

        colorCurrent = 0;
        sliceCurrent = 0;
        colorArea = CtGlobal::images().colorArea();

        while (sliceCurrent < slicesCount-1
               && colorCurrent < m_tube->count())
        {
            nextSegment();
            if (qFuzzyIsNull(colorArea))
                colorArea = CtGlobal::images().colorArea();
        }
    }

    if (!bottleDrawn)
        drawBottle();
}

/*
 * calculates and draws the next color's segment
 */
void old_TubeFlyer::nextSegment()
{
    // gets bottom line from the top of previous segment
    if (sliceCurrent == 0 && colorCurrent == 0) {
        m_bottomLine.x1 = m_bottomLine.x2 = slicesCoordinates[0].x1;
        m_bottomLine.y = slicesCoordinates[0].y;
    } else {
        m_bottomLine.x1 = m_topLine.x1;
        m_bottomLine.x2 = m_topLine.x2;
        m_bottomLine.y = m_topLine.y;
    }
    addColorPoints(m_bottomLine.x1, m_bottomLine.x2, m_bottomLine.y);

    // top line of the segment
    m_topLine.y = slicesCoordinates[sliceCurrent + 1].y;
    m_topLine.x1 = slicesCoordinates[sliceCurrent + 1].x1;
    m_topLine.x2 = slicesCoordinates[sliceCurrent + 1].x2;

    // size (area) of the segment
    qreal dx0 = m_bottomLine.x1 - m_bottomLine.x2; // bottom section length
    qreal dx1 = m_topLine.x1 - m_topLine.x2;       // top section length
    qreal dy = m_bottomLine.y - m_topLine.y;       // height
    sliceArea = dy * (dx0 + dx1) / 2;

    // checks the area
    if (colorArea > sliceArea) {

        // Whole the segment is filled by the current color

        addColorPoints(m_topLine.x1, m_topLine.x2, m_topLine.y);
        colorArea -= sliceArea;
        sliceArea = 0;
        sliceCurrent++;

        if (qFuzzyCompare(m_topLine.y, rotationPoint.y)) // and is this the last segment?
        {
            // draws the color
            drawColorCell();
            clearColorsPoints();
            drawBottle();

            if (!qFuzzyIsNull(endAngle))
                drawFlow(colorArea - sliceArea);

            sliceArea -= colorArea;
            colorArea = 0;
            colorCurrent ++;
        }

    } else {

        // The current segment is not filled by one color,
        // so we need to calculate size of the current color.

        qreal newHeight = dy;
        if (qFuzzyIsNull(dx0)) {
            // The current segment is a triangle.
            newHeight = 2 * colorArea / dx1;

        } else if (qFuzzyIsNull(dx1))  {
            // This is alomst impossible, but let it be.
            // The current segment is a triangle.
            newHeight = 2 * colorArea / dx0;

        }  else if (qFuzzyCompare(dx0, dx1)) {
            // The current segment is a parallelogram.
            newHeight = colorArea / dx1;

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
            qreal D = dx0 * dx0 + 2 * k * colorArea;

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
        addColorPoints(m_topLine.x1, m_topLine.x2, m_topLine.y);
        drawColorCell();
        clearColorsPoints();
        sliceArea -= colorArea;
        colorArea = 0;
        colorCurrent ++;
    }

}

void old_TubeFlyer::drawColorCell()
{
    /*
    grColors.setPaint(palette.getColor(colorCurrent + 5));
    GeneralPath poly = new GeneralPath();

    poly.moveTo(segmentX1[0] + wTrans, segmentY[0] + hTrans);
    for (int i = 1; i< segmentCount; i++) {
        poly.lineTo(segmentX1[i] + wTrans, segmentY[i] + hTrans);
    }
    for (int i = segmentCount; i > 0; i--) {
        poly.lineTo(segmentX2[i-1] + wTrans, segmentY[i-1] + hTrans);
    }
    poly.lineTo(segmentX1[0] + wTrans, segmentY[0] + hTrans);

    poly.closePath();
    grColors.fill(poly);
*/

    QPointF *points = new QPointF[segmentsCount*2+1];
    for (int i = 0; i< segmentsCount; i++) {
        points[i] = QPointF(segmentsCoordinates[i].x1, segmentsCoordinates[i].y);
        points[segmentsCount*2 -1 -i] = QPointF(segmentsCoordinates[i].x2, segmentsCoordinates[i].y);
    }
    points[2*segmentsCount] = points[0];

    QPainter painter(&m_drawImage);
    painter.setBrush(QBrush(CtGlobal::palette().getColor(colorCurrent*2+1)));
    painter.drawPolygon(points, segmentsCount*2+1);
    delete[] points;
}

void old_TubeFlyer::drawFlow(qreal square)
{
    qint16 w = 3;
    qint16 x = rotateLeft?
                round(bottleCoordinates[0].x + hTranslate) + 1 :
                round(bottleCoordinates[0].x + hTranslate) - w - 1;

    qint16 y = round(bottleCoordinates[0].y + vTranslate);
    qint16 h = round(square / w);

    if (h > m_height - y)
        h = m_height - y;
    colorArea -= h * w;

 /*

    grColors.setPaint(palette.getColor(colorCurrent + 5));
    grColors.fillRect(x, y, w, h);

    if ((emptyCount > 1) && (colorCurrent != colorsCount-1) && (h < colors.getHeight() - y)) {
        grColors.setPaint(palette.getColor(colorCurrent + 1 + 5));
        grColors.fillRect(x, y+h, w, colors.getHeight() - (y+h));
    }
 */
}

void old_TubeFlyer::drawBottle()
{

/*
    AffineTransform tx = new AffineTransform();
    tx.translate(100,hTrans);
    tx.rotate(angle, rotationX, rotationY);
    grBottle.drawRenderedImage(bottleOrig, tx);
    grColors.drawImage(bottle, 0, 0, null);
*/
    bottleDrawn = true;
}


void old_TubeFlyer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, m_drawImage);
}




