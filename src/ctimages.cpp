#include "ctimages.h"

#include <QImage>
#include <QtMath>
#include <QPainter>
#include <QDebug>
#include <QJsonObject>

#include "ctglobal.h"
#include "ctio.h"

CtImages* CtImages::m_instance = nullptr;

/*

  Qt 5.6 is not supported SVG transparency masks. So we have to render them manually.

 */

CtImages::CtImages(QObject *parent) :
    QObject(parent)
{
}

CtImages::~CtImages()
{
    delete m_source;
    delete m_bottle;
    delete m_bottleFront;
    delete m_bottleBack;
    delete m_shadeYellow;
    delete m_shadeGreen;
    delete m_shadeBlue;
    delete m_shadeRed;
    delete m_shadeGray;
    delete m_cork;
    delete m_vertices;
    delete [] m_tiltAngles;
    qDebug() << "Images destroyed";
}

CtImages& CtImages::create()
{
    return instance();
}

CtImages& CtImages::instance()
{
    if (m_instance == nullptr) {
        m_instance = new CtImages();
        m_instance->initialize();
        qDebug() << "Images created";
    }
    return * m_instance;
}

void CtImages::initialize()
{
    m_tiltAngles = new qreal[(CT_TUBE_STEPS_POUR << 2) + 1];
    anglesCalculated = false;

    m_source = new QSvgRenderer(QLatin1String(":/img/tube.svg"));
    m_bottle = new QPixmap;
    m_bottleFront = new QPixmap;
    m_bottleBack = new QPixmap;
    m_shadeYellow = new QPixmap;
    m_shadeGreen = new QPixmap;
    m_shadeBlue = new QPixmap;
    m_shadeRed = new QPixmap;
    m_shadeGray = new QPixmap;
    m_cork = new QPixmap;
    m_vertices = new QPointF[6];

    scalePoints();
    renderImages();
}

void CtImages::setScale(qreal value)
{
    if (!qFuzzyCompare(m_scale, value)) {
        m_scale = value;
        scalePoints();
        renderImages();
        emit scaleChanged(m_scale);
    }
    if (!anglesCalculated)
        if (!loadTiltAngles()) {
            calculateTiltAngles();
        }
}

void CtImages::scalePoints()
{
    m_vertices[0] = QPointF(60.0, 15.5) * m_scale;
    m_vertices[1] = QPointF(66.0, 26.0) * m_scale;
    m_vertices[2] = QPointF(66.0, 166.0) * m_scale;
    m_vertices[3] = QPointF(14.0, 166.0) * m_scale;
    m_vertices[4] = QPointF(14.0, 26.0) * m_scale;
    m_vertices[5] = QPointF(20.0, 15.5) * m_scale;

    m_tubeWidth   = 80.0 * m_scale;
    m_shiftWidth  = 100.0 * m_scale;
    m_tubeRotationWidth  = m_tubeWidth + m_shiftWidth;
    m_tubeFullWidth  = m_tubeWidth + m_shiftWidth * 2;

    m_tubeHeight  = 180.0 * m_scale;
    m_shiftHeight = 20.0 * m_scale;
    m_tubeFullHeight = m_tubeHeight + m_shiftHeight;

    m_colorWidth  = m_vertices[2].x() - m_vertices[3].x();
    m_colorHeight = (m_vertices[2].y() - m_vertices[1].y()) / 4;
    m_colorArea   = m_colorWidth * m_colorHeight;

    m_jetWidth    = 3.0 * m_scale;
    m_jetRect     = QRectF(
                m_vertices[3].x() + (m_colorWidth - m_jetWidth) / 2,
                0, m_jetWidth, 0);
}

QRectF CtImages::scaleRect(QRectF rect)
{
    return QRectF(rect.topLeft() * m_scale, rect.size() * m_scale);
}

QRectF CtImages::colorRect(quint8 index)
{
    return QRectF(m_vertices[3].x(),
            m_vertices[3].y() - m_colorHeight * (index+1),
            m_colorWidth,
            m_colorHeight);
}

void CtImages::renderImages()
{
    QRectF elementRect; // rect of the every element in SVG
    QPointF startPoint; // top left point of the elementRect

    QImage image(m_source->defaultSize().rwidth() * m_scale,
                 m_source->defaultSize().rheight() * m_scale,
                 QImage::Format_ARGB32);
    QPainter painter(&image);

    image.fill(0x00ffffff);

//----------------- shine
    elementRect = m_source->boundsOnElement(QLatin1String("shine"));
    elementRect = scaleRect(elementRect);
    startPoint = elementRect.topLeft();
    elementRect.moveTo(0, 0);

    QImage shineImage (elementRect.width(), elementRect.height(), QImage::Format_ARGB32);
    shineImage.fill(0x00ffffff);
    QPainter shinePainter(&shineImage);
    m_source->render(&shinePainter, QLatin1String("shine"), elementRect);

    qreal part;            // part of the image's height/width
    QRgb pix;              // current pixel
    int alpha, newAlpha;   // alpha values of the pixel

    // transparency mask
    for (int y = 0; y < shineImage.height(); ++y) {
        part = qreal(y) / shineImage.height();
        if (part < 0.3) {
            alpha = 0x88;
        } else if (part < 0.9) {
            alpha = 0x88 - round(0x88 / 0.6 * (part - 0.3));
        } else {
            alpha = 0;
        }

        for (int x = 0; x < shineImage.width(); ++x) {
            pix = shineImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = round (qreal(pix >> 24) * alpha / 255.0);
                shineImage.setPixel(x, y, ((newAlpha & 0xff) << 24) | (pix & 0xffffff));
            }
        }
    }
    painter.drawImage(startPoint, shineImage);

//----------------- side
    elementRect = m_source->boundsOnElement(QLatin1String("side"));
    elementRect = scaleRect(elementRect);
    startPoint = elementRect.topLeft();
    elementRect.moveTo(0, 0);

    QImage sideImage (elementRect.width(), elementRect.height(), QImage::Format_ARGB32);
    sideImage.fill(0x00ffffff);
    QPainter sidePainter(&sideImage);
    m_source->render(&sidePainter, QLatin1String("side"), elementRect);

    // transparency mask
    for (int x = 0; x < sideImage.width(); ++x) {
        part = qreal(x) / sideImage.width();
        if (part < 0.2) {
            alpha = 0x44 - round((0x44-0x11) / 0.2 * part);
        } else {
            alpha = 0x11 + round((0x88-0x11) / 0.8 * (part - 0.2));
        }

        for (int y = 0; y < sideImage.height(); ++y) {
            pix = sideImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = round ( qreal(pix >> 24) * alpha / 255.0);
                sideImage.setPixel(x, y, ((newAlpha & 0xff) << 24) | (pix & 0xffffff));
            }
        }
    }
    painter.drawImage(startPoint, sideImage);

//----------------- bottle
    elementRect = m_source->boundsOnElement(QLatin1String("bottle"));
    elementRect = scaleRect(elementRect);
    m_source->render(&painter, QLatin1String("bottle"), elementRect);
    m_bottle->convertFromImage(image);

    // back side
    elementRect = image.rect();
    elementRect.setHeight(m_vertices[0].y());
    *m_bottleBack = m_bottle->copy(elementRect.toRect());

    // front side
    elementRect = image.rect();
    elementRect.adjust(0, m_vertices[0].y(), 0, 0);
    *m_bottleFront = m_bottle->copy(elementRect.toRect());

//----------------- shades
    image.fill(0x00ffffff);
    elementRect  = m_source->boundsOnElement(QLatin1String("shade_yellow"));
    elementRect = scaleRect(elementRect);
    m_source->render(&painter, QLatin1String("shade_yellow"), elementRect);
    m_shadeYellow->convertFromImage(image);

    image.fill(0x00ffffff);
    elementRect  = m_source->boundsOnElement(QLatin1String("shade_green"));
    elementRect = scaleRect(elementRect);
    m_source->render(&painter, QLatin1String("shade_green"), elementRect);
    m_shadeGreen->convertFromImage(image);

    image.fill(0x00ffffff);
    elementRect  = m_source->boundsOnElement(QLatin1String("shade_blue"));
    elementRect = scaleRect(elementRect);
    m_source->render(&painter, QLatin1String("shade_blue"), elementRect);
    m_shadeBlue->convertFromImage(image);

    image.fill(0x00ffffff);
    elementRect  = m_source->boundsOnElement(QLatin1String("shade_red"));
    elementRect = scaleRect(elementRect);
    m_source->render(&painter, QLatin1String("shade_red"), elementRect);
    m_shadeRed->convertFromImage(image);

    image.fill(0x00ffffff);
    elementRect  = m_source->boundsOnElement(QLatin1String("shade_gray"));
    elementRect = scaleRect(elementRect);
    m_source->render(&painter, QLatin1String("shade_gray"), elementRect);
    m_shadeGray->convertFromImage(image);

//----------------- cork
    elementRect  = m_source->boundsOnElement(QLatin1String("cork"));
    elementRect = scaleRect(elementRect);
    QImage corkImage(m_bottle->width(), elementRect.bottom()+1, QImage::Format_ARGB32);
    corkImage.fill(0x00ffffff);
    QPainter corkPainter(&corkImage);
    m_source->render(&corkPainter, QLatin1String("cork"), elementRect);
    m_cork->convertFromImage(corkImage);
}

qreal CtImages::tiltAngle(uint index)
{
    if (index <= 4 * CT_TUBE_STEPS_POUR)
        return m_tiltAngles[index];
    return 0.0;
}

qreal lineLength(QPointF p1, QPointF p2)
{
    return sqrt((p2.x() - p1.x()) * (p2.x() - p1.x())
                + (p2.y() - p1.y()) * (p2.y() - p1.y()));
}

qreal lineAngle(QPointF p1, QPointF p2)
{
    return atan2(p2.y() - p1.y(), p2.x() - p1.x());
}

qreal triangleArea(qreal lineLength, qreal angle1, qreal angle2)
{
    // triangle area by one side and two angles adjacent to it (ASA, angle-side-angle)
    return lineLength * lineLength * qSin(angle1) * qSin(angle2) / qSin(angle1 + angle2) / 2.0;
}

void CtImages::calculateTiltAngles()
{
    qreal l01 = lineLength(m_vertices[0], m_vertices[1]);
    qreal l02 = lineLength(m_vertices[0], m_vertices[2]);
    qreal l03 = lineLength(m_vertices[0], m_vertices[3]);
    qreal l04 = lineLength(m_vertices[0], m_vertices[4]);

    qreal a201 = CT_PI - lineAngle(m_vertices[0], m_vertices[1]);
    qreal a302 = CT_PI - lineAngle(m_vertices[0], m_vertices[2]);
    qreal a403 = CT_PI - lineAngle(m_vertices[0], m_vertices[3]);
    qreal a504 = CT_PI - lineAngle(m_vertices[0], m_vertices[4]);

    qreal a012 = CT_3PI2 - a201;
    qreal a023 = CT_PI - a302;
    qreal a034 = CT_PI2 - a403;
    qreal a045 = CT_PI - a504 - a201;

    struct Triangle {
        qreal side;     // side length
        qreal angle0;   // side angle from the origin
        qreal angle1;   // first angle adjacent to side
        qreal angle2;   // second angle adjacent to side
        qreal area;     // triangle area
    };

    Triangle triangles[4];

    triangles[0].side = l04;
    triangles[0].angle0 = 0;
    triangles[0].angle1 = a504;
    triangles[0].angle2 = a045;
    triangles[0].area = triangleArea(triangles[0].side, triangles[0].angle1, triangles[0].angle2);

    triangles[1].side = l03;
    triangles[1].angle0 = a504;
    triangles[1].angle1 = a403 - a504;
    triangles[1].angle2 = a034;
    triangles[1].area = triangleArea(triangles[1].side, triangles[1].angle1, triangles[1].angle2);

    triangles[2].side = l02;
    triangles[2].angle0 = a403;
    triangles[2].angle1 = a302 - a403;
    triangles[2].angle2 = a023;
    triangles[2].area = triangleArea(triangles[2].side, triangles[2].angle1, triangles[2].angle2);

    triangles[3].side = l01;
    triangles[3].angle0 = a302;
    triangles[3].angle1 = a201 - a302;
    triangles[3].angle2 = a012;
    triangles[3].area = triangleArea(triangles[3].side, triangles[3].angle1, triangles[3].angle2);

    qreal expectedArea;

    int i = 0;
    qreal currentAngle = 0;
    qreal trianglesArea;
    qreal currentArea;

    qreal angleIncrement;

    do {

        angleIncrement = 0.5 * CT_DEG2RAD;
        expectedArea = (CtGlobal::images().colorArea() * 4)
                - (CtGlobal::images().colorArea() * ((qreal) i / (qreal) CT_TUBE_STEPS_POUR));
        if (expectedArea < 0.0) expectedArea = 0.0;

        do {
            int triangleNumber = 3;

            trianglesArea = 0.0;
            while ((currentAngle < triangles[triangleNumber].angle0) && (triangleNumber > 0)) {
                trianglesArea += triangles[triangleNumber].area;
                triangleNumber --;
            }

            currentArea = trianglesArea
                    + triangleArea(triangles[triangleNumber].side,
                                   triangles[triangleNumber].angle1 - (currentAngle - triangles[triangleNumber].angle0),
                                   triangles[triangleNumber].angle2);

            if (qFuzzyIsNull(angleIncrement / 2.0))  {
                currentArea = expectedArea;
            }

            if (!qFuzzyCompare(currentArea, expectedArea)) {
                if (currentArea < expectedArea) {
                    currentAngle -= angleIncrement;
                    angleIncrement = angleIncrement / 2.0;
                } else
                    currentAngle += angleIncrement;
            }

        } while (!qFuzzyCompare(currentArea, expectedArea));

        m_tiltAngles[(CT_TUBE_STEPS_POUR << 2) - i] = currentAngle;
        i++;

    } while (!qFuzzyIsNull(expectedArea));

    m_tiltAngles[1] = (m_tiltAngles[0] + m_tiltAngles[2]) / 2;

    anglesCalculated = true;
    qDebug() << i << "tilt angles were calculated";

    saveTiltAngles();

}

bool CtImages::loadTiltAngles()
{
    QJsonObject jObj, jAngles;

    if (!CtGlobal::io().loadJson(CtGlobal::io().anglesFileName(), jObj))
        return false;

    int steps = 0;
    QString key;
    QString value;

    bool result = jObj.contains("tiltAngles") && jObj["tiltAngles"].isObject();

    if (result) {
        jAngles = jObj["tiltAngles"].toObject();

        if (jAngles.contains("pourStepsPerColor") && jAngles["pourStepsPerColor"].isString())
            steps = jAngles["pourStepsPerColor"].toString().toInt(&result, 10);
        result = result && steps == CT_TUBE_STEPS_POUR;
    }

    if (result) {
        int count = steps * 4 + 1;

        int i = 0;
        while (result && i < count) {

            key = "angle" + CtGlobal::intToStr(i);
            if (jAngles.contains(key) && jAngles[key].isString()) {

                value = jAngles[key].toString();
                m_tiltAngles[i] = value.toDouble(&result) * CT_DEG2RAD;
                if (!result)
                    qDebug() << key << "cannot read";
            }
            i++;
        }
    }

    if (result) {
        anglesCalculated = true;
        qDebug() << CT_TUBE_STEPS_POUR * 4 + 1 << "tilt angles were restored";
    }

    return result;
}

bool CtImages::saveTiltAngles()
{
    // creates json
    QJsonObject jItem;
    QJsonObject jObj;
    QString value;

    // add count
    jItem["pourStepsPerColor"] = CtGlobal::intToStr(CT_TUBE_STEPS_POUR);

    // add angles
    for (int i = 0; i < CT_TUBE_STEPS_POUR * 4 + 1; i++) {
        value = QString().number(m_tiltAngles[i] * CT_RAD2DEG, 'g', 16);
        jItem["angle"+CtGlobal::intToStr(i)] = value;
    }

    jObj["tiltAngles"] = jItem;

    // save
    if (CtGlobal::io().saveJson(CtGlobal::io().anglesFileName(), jObj)) {
        qDebug() << (CT_TUBE_STEPS_POUR << 2) + 1 << "tilt angles were saved";
        return true;
    }

    return false;
}
