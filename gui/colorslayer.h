#ifndef COLORSLAYER_H
#define COLORSLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>
#include <QPainter>

// class TubeModel;
class TubeItem;

class ColorsLayer : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit ColorsLayer(TubeItem *parent = 0);
    ~ColorsLayer();

    void  refresh();

// pouring routines must be available from TubeItem class
    void  resetPourArea() {m_pourArea = 0;}
    void  addPourArea(qreal areaIncrement, quint8 colorIndex);
    void  addPourArea(qreal areaIncrement, qreal jetArea, quint8 colorIndex);
//    qreal getPouringArea() {return m_pouringArea;}


private slots:
    void  onScaleChanged();
    void  onAngleChanged();

private:
    TubeItem  * m_tube;                     // parent tube for this layer
    void paint(QPainter * painter) override;

    qreal       scale();

//  draw colors
    QImage    * m_drawImage;
    QPainter  * m_painter;
    void        drawColors();

//  fill colors from another tube
    qreal       m_fillArea;                  // currently filled area
    QRectF      m_colorRect;                 // rect of the color
    qreal       m_pourArea;

//  drop colors to another tube
    void        nextSegment();
    void        drawColorCell();

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
        qreal x1;
        qreal y1;
        qreal x2;
        qreal y2;
        qreal k;    // koeff k
        qreal b;    // koeff b
    };

    PointF    * tubeVertices;                      // coordinates after rotation
    LineF     * bottleLines;                       // bottle edge lines
    qreal       getIntersectionX(quint8 vertex);   // the intersection of the horizontal line from bottle vertices

    SliceF    * tubeSlices;                         // tube is sliced by horizontal lines passing through its vertices
    qint8       m_slicesCount;
    qint8       m_sliceIndex;
    void        addSlice(qint8 vertex, qreal x1, qreal x2, qreal y);
    void        clearSlices();

    SliceF    * colorSegments;                       // color segments coordinates
    qint8       m_segmentsCount;
    qint8       m_colorIndex;
    qreal       m_segmentArea;
    void        addColorSegment(SliceF line);
    void        clearColorSegments();

    SliceF      m_bottomLine;
    SliceF      m_topLine;
};

#endif // COLORSLAYER_H
