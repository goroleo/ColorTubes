#ifndef COLORSLAYER_H
#define COLORSLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>
#include <QPainter>

class TubeItem;

class ColorsLayer : public QQuickPaintedItem
{
    Q_OBJECT
    friend class TubeItem;

public:
    explicit ColorsLayer(TubeItem *parent = 0);
    ~ColorsLayer();

    void  refresh();

private slots:
    void  onScaleChanged();
    void  onAngleChanged();

private:
    TubeItem  * m_tube;                      // parent tube for this layer
    void        paint(QPainter * painter) override;

    //  draw colors
    QImage    * m_drawImage;
    QPainter  * m_painter;
    QRectF      m_colorRect;                 // rect of the color
    qreal       m_fillArea;                  // currently filled area
    void        drawColors();

    //  fill colors from another tube
    void        resetFillArea() { m_fillArea = 0.0; }
    void        addFillArea(qreal areaIncrement, quint8 colorIndex);
    void        addFillArea(qreal areaIncrement, qreal jetArea, quint8 colorIndex);

    //  rotation coordinates & routines
    struct PointF {
        qint8 v;    // vertex/point number
        qreal x;    // X coorinate
        qreal y;    // Y coorinate
    };

    struct SliceF {
        qint8 v;    // vertex/point number
        qreal x1;   // left X
        qreal x2;   // right X
        qreal y;    // Y coordinate
    };
    
    struct LineF {  // line is y = k * x + b
        qreal x1;   // point1.x
        qreal y1;   // point1.y
        qreal x2;   // point2.x
        qreal y2;   // point2.y
        qreal k;    // koeff k
        qreal b;    // koeff b
    };

    void        nextSegment();
    void        drawColorCell();

    PointF    * tubeVertices;                // coordinates after rotation
    LineF     * bottleLines;                 // bottle edge lines
    qreal       intersectByX(quint8 vertex); // the intersection of the horizontal line from bottle vertices

    SliceF    * tubeSlices;                  // tube is sliced by horizontal lines passing through its vertices
    qint8       m_slicesCount;
    qint8       m_sliceIndex;
    void        addSlice(qint8 vertex, qreal x1, qreal x2, qreal y);
    void        clearSlices();

    SliceF    * colorSegments;               // color segments coordinates
    qint8       m_segmentsCount;
    qint8       m_colorIndex;
    qreal       m_segmentArea;
    void        addColorSegment(SliceF line);
    void        clearColorSegments();

    SliceF      m_bottomLine;
    SliceF      m_topLine;
};

#endif // COLORSLAYER_H
