#ifndef TUBEIMAGES_H
#define TUBEIMAGES_H

#include <QtGlobal>
#include <QtSvg/QSvgRenderer>
#include <QPixmap>

// Qt is not supported SVG transparency masks. So we have to render them manually.
class TubeImages : public QObject
{
    Q_OBJECT
public:
    static TubeImages & create();
    static TubeImages & instance();

    ~TubeImages();

    QPixmap bottle()
    {
        return *m_bottle;
    }

    QPixmap bottleBack()
    {
        return *m_bottleBack;
    }

    QPixmap bottleFront()
    {
        return *m_bottleFront;
    }

    QPixmap shadeYellow()
    {
        return *m_shadeYellow;
    }

    QPixmap shadeGreen()
    {
        return *m_shadeGreen;
    }

    QPixmap shadeBlue()
    {
        return *m_shadeBlue;
    }


    QPixmap cork()
    {
        return *m_cork;
    }

    qreal scale()
    {
        return m_scale;
    };

    qreal width()
    {
        return m_bottle->width();
    };

    qreal height()
    {
        return m_bottle->height();
    };

    QPointF * vertices()
    {
        return m_vertices;
    };

    QPointF center()
    {
        return m_centerPoint;
    }

    QPointF vertex(quint8 index)
    {
        return m_vertices[index];
    };

    QRectF colorRect(quint8 index);

    qreal colorHeight() {
        return m_colorHeight;
    }

    qreal colorWidth() {
        return m_colorWidth;
    }

    qreal colorArea() {
        return m_colorArea;
    }

    QRectF scaleRect (QRectF rect);

public slots:
    void setScale(qreal value);

signals:
    void scaleChanged(qreal newScale);

private:
    explicit TubeImages(QObject *parent = nullptr);

    void initialize();
    static TubeImages* m_instance;

    void scalePoints();
    void renderImages();

    qreal m_scale = 1.0;
    QSvgRenderer *m_source;

    QPixmap *m_bottle;
    QPixmap *m_bottleBack;
    QPixmap *m_bottleFront;
    QPixmap *m_shadeYellow;
    QPixmap *m_shadeGreen;
    QPixmap *m_shadeBlue;
    QPixmap *m_cork;

    QPointF *m_vertices;   // vertices of the егиу
    QPointF m_centerPoint;   // central point to draw tube from it

    qreal m_colorHeight; // width of the one color cell after scaling
    qreal m_colorWidth;
    qreal m_colorArea;   // colorWidth * colorHeight
};

#endif // TUBEIMAGES_H
