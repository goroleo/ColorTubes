#include "ctimages.h"

#include <QImage>
#include <QPainter>
#include <QDebug>
#include <QJsonObject>

#include "ctglobal.h"
#include "ctio.h"

CtImages * CtImages::m_instance = nullptr;

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
        qDebug() << "Creating tube images.";
        m_instance = new CtImages();
        m_instance->initialize();
    }
    return * m_instance;
}

void CtImages::initialize()
{
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

    m_tiltAngles = new qreal[(CT_TUBE_STEPS_POUR * 4) + 1];
    anglesExist = false;

    scaleVertices();
    renderImages();
}

void CtImages::setScale(qreal value)
{
    if (!qFuzzyCompare(m_scale, value)) {
        m_scale = value;
        scaleVertices();
        renderImages();
        emit scaleChanged(m_scale);
    }
    if (!anglesExist)
        if (!loadTiltAngles()) {
            calculateTiltAngles();
            saveTiltAngles();
        }
}

void CtImages::scaleVertices()
{
/*
           5 +  neck  + 0
            /          \
           /            \
        4 +              + 1
          |              |
          |     tube     |
          |              |
        3 + ------------ + 2

        (see /img/tube.svg)
*/

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
            alpha = 0x88 - qRound(0x88 / 0.6 * (part - 0.3));
        } else {
            alpha = 0;
        }

        for (int x = 0; x < shineImage.width(); ++x) {
            pix = shineImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = qRound (qreal(pix >> 24) * alpha / 255.0);
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
            alpha = 0x44 - qRound((0x44-0x11) / 0.2 * part);
        } else {
            alpha = 0x11 + qRound((0x88-0x11) / 0.8 * (part - 0.2));
        }

        for (int y = 0; y < sideImage.height(); ++y) {
            pix = sideImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = qRound ( qreal(pix >> 24) * alpha / 255.0);
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
    if (anglesExist && index <= 4 * CT_TUBE_STEPS_POUR)
        return m_tiltAngles[index];
    return 0.0;
}

void CtImages::calculateTiltAngles()
{
/*
    The tube rotates clockwise around point 0 and pours out its colors at this point.
    The area filled with colors depends on the rotation/tilt angle.
    Each of 4 colors is poured out evenly in <CT_TUBE_STEPS_POUR> steps (see CtGlobal).
    This routine calculates each tilt angle for each step.

    Based on 4 triangles inside the tube: 504, 403, 302, 201.

           5 +  neck  + 0
            /          \
           /            \
        4 +              + 1
          |              |
          |     tube     |
          |              |
        3 + ------------ + 2
*/

//  l01 - line between point 0 and point 1;
//  l02 - line between point 0 and point 2, etc.
    qreal l01 = lineLength(m_vertices[0], m_vertices[1]);
    qreal l02 = lineLength(m_vertices[0], m_vertices[2]);
    qreal l03 = lineLength(m_vertices[0], m_vertices[3]);
    qreal l04 = lineLength(m_vertices[0], m_vertices[4]);

//  a501 - angle at point 0 between line l50 (l05) and line l01, etc.
    qreal a501 = - lineAngle(m_vertices[1], m_vertices[0]);
    qreal a502 = - lineAngle(m_vertices[2], m_vertices[0]);
    qreal a503 = - lineAngle(m_vertices[3], m_vertices[0]);
    qreal a504 = - lineAngle(m_vertices[4], m_vertices[0]);

//  a012 - angle at point 1 between line l01 and line l12, etc.
    qreal a012 = CT_3PI2 - a501;
    qreal a023 = CT_PI - a502;
    qreal a034 = CT_PI2 - a503;
    qreal a045 = CT_PI - a504 - a501;

    struct Triangle {
        qreal side;     // side length
        qreal angleZ;   // Zero Angle - side angle from the origin
        qreal angle1;   // first angle adjacent to side
        qreal angle2;   // second angle adjacent to side
        qreal area;     // triangle area
    };
    Triangle triangles[4];

//  Triangle 504
    triangles[0].side = l04;
    triangles[0].angleZ = 0;
    triangles[0].angle1 = a504;
    triangles[0].angle2 = a045;
    triangles[0].area = triangleArea(triangles[0].side, triangles[0].angle1, triangles[0].angle2);

//  Triangle 403
    triangles[1].side = l03;
    triangles[1].angleZ = a504;
    triangles[1].angle1 = a503 - a504;
    triangles[1].angle2 = a034;
    triangles[1].area = triangleArea(triangles[1].side, triangles[1].angle1, triangles[1].angle2);

//  Triangle 302
    triangles[2].side = l02;
    triangles[2].angleZ = a503;
    triangles[2].angle1 = a502 - a503;
    triangles[2].angle2 = a023;
    triangles[2].area = triangleArea(triangles[2].side, triangles[2].angle1, triangles[2].angle2);

//  Triangle 201
    triangles[3].side = l01;
    triangles[3].angleZ = a502;
    triangles[3].angle1 = a501 - a502;
    triangles[3].angle2 = a012;
    triangles[3].area = triangleArea(triangles[3].side, triangles[3].angle1, triangles[3].angle2);

    qreal currentAngle = 0.0;
    qreal angleIncrement;

    qreal expectedArea;    // colors area at each step
    qreal baseArea;        // area of triangles which are completely laid below the current angle
    qreal calculatedArea;  // colors area at current angle

//  Calculate angles
    for (int i = 0; i < CT_TUBE_STEPS_POUR * 4; ++i) {

        angleIncrement = 0.5 * CT_DEG2RAD;
        expectedArea = (CtGlobal::images().colorArea() * 4)
                - (CtGlobal::images().colorArea() * (qreal (i) / qreal (CT_TUBE_STEPS_POUR)));
        if (expectedArea < 0.0) expectedArea = 0.0;

        do {
            int triangleNumber = 3;

            baseArea = 0.0;
            while ((currentAngle < triangles[triangleNumber].angleZ) && (triangleNumber > 0)) {
                baseArea += triangles[triangleNumber].area;
                triangleNumber --;
            }

            calculatedArea = baseArea
                    + triangleArea(triangles[triangleNumber].side,
                                   triangles[triangleNumber].angle1 - (currentAngle - triangles[triangleNumber].angleZ),
                                   triangles[triangleNumber].angle2);

            if (qFuzzyIsNull(angleIncrement / 2.0))  {
                calculatedArea = expectedArea;
            }

            if (!qFuzzyCompare(calculatedArea, expectedArea)) {
                if (calculatedArea < expectedArea) {
                    currentAngle -= angleIncrement;
                    angleIncrement /= 2.0;
                } else
                    currentAngle += angleIncrement;
            }

        } while (!qFuzzyCompare(calculatedArea, expectedArea));

        m_tiltAngles[(CT_TUBE_STEPS_POUR * 4) - i] = currentAngle;
    }

    m_tiltAngles[0] = a501;
    m_tiltAngles[1] = (m_tiltAngles[0] + m_tiltAngles[2]) / 2;

    anglesExist = true;
    qDebug() << (CT_TUBE_STEPS_POUR * 4) + 1 << "tilt angles were calculated";
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
                    qDebug() << "Cannot read value at" << key;
            } else {
                qDebug() << "Cannot find the key" << key;
            }
            i++;
        }
    }

    if (result) {
        qDebug() << CT_TUBE_STEPS_POUR * 4 + 1 << "tilt angles were restored from" << CtGlobal::io().anglesFileName();
    } else {
        qDebug() << "Error while loading tilt angles from" << CtGlobal::io().anglesFileName();
    }

    anglesExist = result;
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
        jItem["angle" + CtGlobal::intToStr(i)] = value;
    }

    jObj["tiltAngles"] = jItem;

    // save
    if (CtGlobal::io().saveJson(CtGlobal::io().anglesFileName(), jObj)) {
        qDebug() << (CT_TUBE_STEPS_POUR << 2) + 1 << "tilt angles were saved";
        return true;
    }

    return false;
}

qreal CtImages::lineLength(QPointF p1, QPointF p2)
{
    return sqrt((p2.x() - p1.x()) * (p2.x() - p1.x())
                + (p2.y() - p1.y()) * (p2.y() - p1.y()));
}

qreal CtImages::lineAngle(QPointF p1, QPointF p2)
{
    return atan2(p2.y() - p1.y(), p2.x() - p1.x());
}

qreal CtImages::triangleArea(qreal lineLength, qreal angle1, qreal angle2)
{
    // triangle area by one side and two angles adjacent to it (ASA, angle-side-angle)
    return lineLength * lineLength * qSin(angle1) * qSin(angle2) / qSin(angle1 + angle2) / 2.0;
}
