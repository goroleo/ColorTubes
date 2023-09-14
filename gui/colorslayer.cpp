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
    m_count = 1;

// tilt angles
    m_tiltAngles = new qreal[5];
    m_tiltAngles[0] = 118.6105 / 180 * M_PI;
    m_tiltAngles[1] = 85.252   / 180 * M_PI;
    m_tiltAngles[2] = 77.666   / 180 * M_PI;
    m_tiltAngles[3] = 71.5285  / 180 * M_PI;
    m_tiltAngles[4] = 26.9970  / 180 * M_PI;

// coordinates
    bottleCoordinates = new PointF[6];
    bottlePolarCoordinates = new PointP[6];
    edgeLines = new QLineF[4];
    slicesCoordinates = new SegmentCoo[6];
    segmentsCoordinates = new SegmentCoo[6];


    m_drawImage = new QImage(280, 200, QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
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


    delete [] bottlePolarCoordinates;
    delete [] bottleCoordinates;
    delete [] edgeLines;
    delete [] slicesCoordinates;
    delete [] segmentsCoordinates;

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
}

void ColorsLayer::fillColors(quint8 colorNum, quint8 count)
{
    if (m_rotateTimer->isActive())
        return;

    if (m_fillTimer->isActive())
    {
        if (m_count == 4)
        {
            m_count = 0;
            drawColors();
            update();
            return;
        }

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

// set rotation point
    quint8 rPointNumber;
    if (m_angle > 0)
        rPointNumber = 0;
    else
        rPointNumber = 5;

    rotationPoint.pointNumber = rPointNumber;
    rotationPoint.x = CtGlobal::images().point(rPointNumber).x();
    rotationPoint.y = CtGlobal::images().point(rPointNumber).y();

    bottleCoordinates[0].pointNumber = 0;
    bottleCoordinates[0].x = rotationPoint.x;
    bottleCoordinates[0].y = rotationPoint.y;

// calculates vertices polar coordinates
    for (int i = 0; i < 6; i++)
    {
        qreal dx = CtGlobal::images().point(i).x() - rotationPoint.x;
        qreal dy = CtGlobal::images().point(i).y() - rotationPoint.y;
        bottlePolarCoordinates[abs(rPointNumber - i)].r = sqrt(dx*dx + dy*dy);
        bottlePolarCoordinates[abs(rPointNumber - i)].a = atan2(dy, dx);
    }

// rotate vertices
    for (qint8 i = 1; i < 6; i++)
    {
        bottleCoordinates[i].x = rotationPoint.x + bottlePolarCoordinates[i].r
                * qCos(bottlePolarCoordinates[i].a + m_angle);
        bottleCoordinates[i].y = rotationPoint.y + bottlePolarCoordinates[i].r
                * qSin(bottlePolarCoordinates[i].a + m_angle);
        bottleCoordinates[i].pointNumber = i;
    }

// calculate edges before sort points
    qDebug() << "edges:";
    for (qint8 i = 0; i < 4; i++)
    {
        edgeLines[i] = QLineF(bottleCoordinates[i].x, bottleCoordinates[i].y,
                              bottleCoordinates[i+1].x, bottleCoordinates[i+1].y);

        qDebug() << "edge:" << i
                 << "p1_num:" << bottleCoordinates[i].pointNumber
                 << "p2_num:" << bottleCoordinates[i+1].pointNumber
                 << "p1:"     << edgeLines[i].p1()
                 << "p2:"     << edgeLines[i].p2();
    }

// sort points by vertical
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

    qDebug() << "after points sort:";
    for (int i = 0; i < 6; ++i) {
        qDebug() << "point:" << i
                 << "p_num:" << bottleCoordinates[i].pointNumber
                 << "x:"     << bottleCoordinates[i].x
                 << "y:"     << bottleCoordinates[i].y;
    }

// calculate slices
    clearSlicesPoints();
    if (bottleCoordinates[0].pointNumber != 0             // rotation point is not a lowest point
            && !qFuzzyIsNull(qAbs(m_angle) - M_PI/2))     // angle != +-90 deg
    {

        // add lowest point
        addSlicePoints(bottleCoordinates[0].pointNumber, bottleCoordinates[0].x, bottleCoordinates[0].x, bottleCoordinates[0].y);

        quint8 currentPoint = 0;
        do {
            currentPoint++;
            qreal x = getIntersection(currentPoint);
            addSlicePoints(bottleCoordinates[currentPoint].pointNumber,
                           x, bottleCoordinates[currentPoint].x,
                           bottleCoordinates[currentPoint].y);
        } while (bottleCoordinates[currentPoint].pointNumber != rotationPoint.pointNumber);
    }

    qDebug() << "slicing:" << m_slicesCount;
    for (int i = 0; i < m_slicesCount; ++i) {
        qDebug() << "slice:" << i
                 << "p_num:" << slicesCoordinates[i].pointNumber
                 << "x1:"    << slicesCoordinates[i].x1
                 << "x2:"    << slicesCoordinates[i].x2
                 << "y:"     << bottleCoordinates[i].y;
    }
    emit angleChanged(m_angle);
}


void ColorsLayer::addSlicePoints(qint8 pNumber, qreal x1, qreal x2, qreal y)
{
    slicesCoordinates[m_slicesCount].pointNumber = pNumber;
    slicesCoordinates[m_slicesCount].x1 = qMin(x1, x2);
    slicesCoordinates[m_slicesCount].x2 = qMax(x1, x2);
    slicesCoordinates[m_slicesCount].y = y;
    m_slicesCount++;
}

void ColorsLayer::clearSlicesPoints()
{
    for (int i = 0; i < 6; i++)
    {
        slicesCoordinates[segmentsCount].pointNumber = 0;
        slicesCoordinates[segmentsCount].x1 = 0;
        slicesCoordinates[segmentsCount].x2 = 0;
        slicesCoordinates[segmentsCount].y = 0;
    }
    m_slicesCount = 0;
}

qreal ColorsLayer::getIntersection(quint8 point)
{

    QLineF horizLine = QLineF(0,           bottleCoordinates[point].y,
                              280*scale(), bottleCoordinates[point].y);
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
