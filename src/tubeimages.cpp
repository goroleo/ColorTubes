#include "tubeimages.h"

#include <QImage>
#include <QtMath>
#include <QPainter>

#include <QDebug>
#include "ctglobal.h"

TubeImages* TubeImages::m_instance = nullptr;

/*

  Qt is not supported SVG transparency masks. So we have to render them manually.

 */

TubeImages::TubeImages(QObject *parent) :
    QObject(parent)
{
}

TubeImages::~TubeImages()
{
    delete m_source;
    delete m_bottle;
    delete m_shadeYellow;
    delete m_shadeGreen;
    delete m_shadeBlue;
    delete m_cork;
    delete m_points;
}

TubeImages& TubeImages::create()
{
    if (m_instance == nullptr) {
        m_instance = new TubeImages();
        m_instance->initialize();
    }
    return *m_instance;
}

void TubeImages::initialize()
{
    m_source = new QSvgRenderer(QLatin1String(":/img/tube.svg"));
    m_bottle = new QPixmap;
    m_shadeYellow = new QPixmap;
    m_shadeGreen = new QPixmap;
    m_shadeBlue = new QPixmap;
    m_cork = new QPixmap;
    m_points = new QPointF[6];

    renderImages();
    scalePoints();
}

TubeImages& TubeImages::instance()
{
    return *m_instance;
}

TubeImages* TubeImages::object() const
{
    return m_instance;
}

QRectF TubeImages::colorRect(quint8 index)
{
    QRectF result;
    result.setX(m_points[3].x());
    result.setY(m_points[3].y() - m_colorHeight * index);
    result.setHeight(m_colorHeight);
    result.setWidth(m_colorWidth);
    return result;
}

void TubeImages::setScale(qreal newScale)
{
    if (!qFuzzyCompare(m_scale, newScale))
    {
        m_scale = newScale;
        renderImages();
        scalePoints();
        emit scaleChanged(newScale);
    }
}

void TubeImages::scalePoints()
{
    m_points[0] = QPointF(60.0, 15.5) * m_scale;
    m_points[1] = QPointF(66.0, 26.0) * m_scale;
    m_points[2] = QPointF(66.0, 166.0) * m_scale;
    m_points[3] = QPointF(14.0, 166.0) * m_scale;
    m_points[4] = QPointF(14.0, 26.0) * m_scale;
    m_points[5] = QPointF(20.0, 15.5) * m_scale;
    m_colorHeight = (m_points[2].y() - m_points[1].y()) / 4;
    m_colorWidth = m_points[2].x() - m_points[2].x();
    m_colorArea = m_colorWidth * m_colorHeight;
}

QRectF TubeImages::scaleRect(QRectF rect)
{
    QRectF result;
    result.setTopLeft(rect.topLeft() * m_scale);
    result.setSize(rect.size() * m_scale);
    return result;
}

void TubeImages::renderImages()
{
    QRectF elementRect; // rect of the every element in SVG
    QPointF startPoint; // top left point of the elementRect

    QImage image(m_source->defaultSize().rwidth() * m_scale,
                 m_source->defaultSize().rheight() * m_scale,
                 QImage::Format_ARGB32);
    QPainter painter(&image);

    image.fill(0x00ffffff);
    QRgb pix;
    int alpha, newAlpha;

//----------------- shine
    elementRect = m_source->boundsOnElement(QLatin1String("shine"));
    elementRect = scaleRect(elementRect);
    startPoint = elementRect.topLeft();
    elementRect.moveTo(0, 0);

    QImage shineImage (elementRect.width(), elementRect.height(), QImage::Format_ARGB32);
    shineImage.fill(0x00ffffff);
    QPainter shinePainter(&shineImage);
    m_source->render(&shinePainter, QLatin1String("shine"), elementRect);

    // transparency mask
    for (int y = 0; y < shineImage.height(); ++y) {
        qreal dy = qreal(y) / shineImage.height();
        if (dy < 0.3) {
            alpha = 0x88;
        } else if (dy < 0.9) {
            alpha = 0x88 - round(0x88 / 0.6 * (dy-0.3));
        } else {
            alpha = 0;
        }

        for (int x = 0; x < shineImage.width(); ++x) {
            pix = shineImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = round (qreal(pix >> 24) * alpha / 255);
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
        qreal dx = qreal(x) / sideImage.width();
        if (dx < 0.2) {
            alpha = 0x44 - round((0x44-0x11) / 0.2 * dx);
        } else {
            alpha = 0x11 + round((0x88-0x11) / 0.8 * (dx - 0.2));
        }

        for (int y = 0; y < sideImage.height(); ++y) {
            pix = sideImage.pixel(x, y);
            if ((pix >> 24) > 0) {
                newAlpha = round ( qreal(pix >> 24) * alpha / 255);
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

//----------------- cork
    elementRect  = m_source->boundsOnElement(QLatin1String("cork"));
    elementRect = scaleRect(elementRect);
    QImage corkImage(m_bottle->width(), elementRect.bottom()+1, QImage::Format_ARGB32);
    corkImage.fill(0x00ffffff);
    QPainter corkPainter(&corkImage);
    m_source->render(&corkPainter, QLatin1String("cork"), elementRect);
    m_cork->convertFromImage(corkImage);
}

