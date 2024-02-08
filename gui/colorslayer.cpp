#include "colorslayer.h"

#include <QPainter>
#include <QBrush>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/game.h"
#include "src/tubeimages.h"
#include "src/palette.h"
#include "core/tubemodel.h"
#include "tubeitem.h"

ColorsLayer::ColorsLayer(TubeItem * parent, TubeModel * tm) :
    QQuickPaintedItem((QQuickItem *) parent),
    m_model(tm)
{

    parentTube = parent;

// tilt angles
    m_tiltAngles = new qreal[5];
    m_tiltAngles[0] = 118.6105 * CT_DEG2RAD;
    m_tiltAngles[1] = 85.252   * CT_DEG2RAD;
    m_tiltAngles[2] = 77.666   * CT_DEG2RAD;
    m_tiltAngles[3] = 71.5285  * CT_DEG2RAD;
    m_tiltAngles[4] = 26.9970  * CT_DEG2RAD;

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
        addFillArea(m_fillAreaInc);
        update();
    });

/*
    m_rotateTimer = new QTimer(this);
    connect(m_rotateTimer, &QTimer::timeout, [=]() {
        addAngle(m_angleInc);
        update();
    });
*/

    if (m_model) {
        drawColors();
        update();
    }

    m_Areas = new qreal[4];
}

ColorsLayer::~ColorsLayer()
{
    delete [] m_tiltAngles;
    delete m_painter;
    delete m_drawImage;
    delete m_fillTimer;
//  delete m_rotateTimer;

    delete [] tubeVertices;
    delete [] bottleLines;
    delete [] tubeSlices;
    delete [] colorSegments;

    delete [] m_Areas;
}

void ColorsLayer::setModel(TubeModel *tm)
{
    m_model = tm;
    drawColors();
    update();
}

qreal ColorsLayer::scale()
{
    return CtGlobal::images().scale();
}

qreal ColorsLayer::angle()
{
    return parentTube->angle();
}

int ColorsLayer::count()
{
    return m_model->count();
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

    qDebug() << "Colors::onscaleChanged" << scale();

//     emit scaleChanged(scale());
}

void ColorsLayer::onAngleChanged()
{
    setAngle(parentTube->angle());
}

void ColorsLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, * m_drawImage);
}

void ColorsLayer::drawColors()
{

    m_drawImage->fill(0x00ffffff);
    if (!m_model)
        return;

    if (qFuzzyIsNull(angle()))
    {
        for (quint8 i = 0; i < m_model->count(); ++i)
        {
            m_colorRect = CtGlobal::images().colorRect(i);
//            m_colorRect.translate(100 * scale(), 20 * scale());
            m_painter->fillRect(m_colorRect, CtGlobal::palette().getColor(m_model->getColor(i)));
        }
    }
    else
    {
        m_colorCurrent = 0;
        m_sliceCurrent = 0;

        m_Areas[0] = 0.0;
        m_Areas[1] = 0.0;
        m_Areas[2] = 0.0;
        m_Areas[3] = 0.0;

        clearColorSegments();

        m_fillArea = CtGlobal::images().colorArea();

        while (m_sliceCurrent < m_slicesCount-1
               && m_colorCurrent < m_model->count())
        {
            nextSegment();
            if (qFuzzyIsNull(m_fillArea))
                m_fillArea = CtGlobal::images().colorArea();
        }

        qDebug() << angle() * CT_RAD2DEG << ";"
                 << m_Areas[0] << ";" << CtGlobal::images().colorArea() - m_Areas[0] << ";"
                 << m_Areas[1] << ";" << CtGlobal::images().colorArea() - m_Areas[1] << ";"
                 << m_Areas[2] << ";" << CtGlobal::images().colorArea() - m_Areas[2] << ";"
                 << m_Areas[3] << ";" << CtGlobal::images().colorArea() - m_Areas[3] ;

    }

}

/*
void ColorsLayer::fillColors(quint8 colorNum, quint8 count)
{
    if (!m_model)
        return;

    if (!qFuzzyIsNull(angle()) || m_rotateTimer->isActive())
        return;

    count = qMin(count, quint8 (4 - m_model->count()));
    if (count == 0)
        return;

    if (m_fillTimer->isActive())
    {
        if (m_model->getColor(m_model->count() - 1) == colorNum)
            m_fillCount += count;
    }
    else
    {
        drawColors();
        update();

        m_fillArea = 0;
        m_fillAreaInc = CtGlobal::images().colorArea() / 40;
        m_fillColor = CtGlobal::palette().getColor(colorNum);
        m_fillCount = count;
        m_fillJetWidth = 3 * scale();

        m_colorRect = CtGlobal::images().colorRect(m_model->count());
        m_colorRect.translate(100 * scale(), 20 * scale());
        m_colorBottom = m_colorRect.bottom();

        m_fillTimer->start(20);
    }

}
*/

void ColorsLayer::dropColors(quint8 count)
{
    if (!m_model)
        return;

//    if (m_fillTimer->isActive() || m_rotateTimer->isActive())
//        return;

    m_dropCount = qMin(count, m_model->count());
    if (m_dropCount == 0)
        return;

    m_startAngle = m_tiltAngles[m_model->count()];
    setAngle(m_tiltAngles[m_model->count()]);
    m_endAngle = m_tiltAngles[m_model->count() - m_dropCount];

//    m_angleInc = (m_endAngle - m_startAngle) / 60 / count;
//    m_rotateTimer->start(20);
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


/*
void ColorsLayer::addAngle(qreal angleInc)
{
    setAngle(m_angle + angleInc);
    if (qFuzzyCompare(m_angle, m_endAngle)) {
        m_angleInc = - m_angle / 10;
    } else if (qFuzzyIsNull(m_angle))
        m_rotateTimer->stop();
}

*/
void ColorsLayer::setAngle(qreal newAngle)
{

    if (qFuzzyIsNull(newAngle))
    {
        drawColors();
        update();
        return;
    }

// ---- set rotation point
    quint8 rVertexNumber; // rotation vertex number
    if (newAngle > 0)
        rVertexNumber = 0;
    else
        rVertexNumber = 5;

    tubeVertices[0].v = 0;
    tubeVertices[0].x = CtGlobal::images().vertex(rVertexNumber).x();
    tubeVertices[0].y = CtGlobal::images().vertex(rVertexNumber).y();

// --- calculate new coordinates of other vertices after rotation
    qreal cos = qCos(newAngle);
    qreal sin = qSin(newAngle);

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

// --- calculate slices
    clearSlices();
    if (tubeVertices[0].v != 0)             // rotation point must be not the lowest point
    {
        quint8 currentVertex;

        // lowest point(s)
        if ( qFuzzyCompare(qAbs(newAngle), (qreal) CT_PI2) )
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
        m_Areas[m_colorCurrent] += sliceArea;
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
             * The current segment is a trapeze or triangle.
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
        m_Areas[m_colorCurrent] += m_fillArea;
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

    path.translate(100 * scale(), 20 * scale());
    m_painter->setBrush(QBrush(CtGlobal::palette().getColor(m_model->getColor(m_colorCurrent))));
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

