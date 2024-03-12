#ifndef COLORSLAYER_H
#define COLORSLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>
#include <QPainter>

class TubeModel;
class TubeItem;

class ColorsLayer : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit ColorsLayer(TubeItem *parent = 0);
    ~ColorsLayer();

    void paint(QPainter * painter) override;
    void refresh();

private slots:
    void onScaleChanged();
    void onAngleChanged();

private:
    TubeItem  * m_tube;

    qreal       scale();
//    qreal       angle();
    TubeModel * model();

//  draw colors
    void        drawColors();
    QImage    * m_drawImage;
    QPainter  * m_painter;

//  fill colors from another tube
    void        addFillArea(qreal fillAreaInc);
    QTimer    * m_fillTimer;
    qreal       m_fillArea;                  // currently filled area
    QRectF      m_colorRect;                 // rect of the color

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
        qint8 v;    // vertex number
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

    void        addSlice(qint8 vertex, qreal x1, qreal x2, qreal y);
    void        clearSlices();
    SliceF    * tubeSlices;                         // tube is sliced by horizontal lines passing through its vertices
    qint8       m_slicesCount;
    qint8       m_sliceCurrent;

    void        addColorSegment(SliceF line);
    void        clearColorSegments();
    SliceF    * colorSegments;                       // color segments coordinates
    qint8       m_segmentsCount;
    qint8       m_colorCurrent;
    qreal       m_segmentArea;

    SliceF      m_bottomLine;
    SliceF      m_topLine;

    // temporary
//    qreal  * m_Areas;

};

#endif // COLORSLAYER_H
