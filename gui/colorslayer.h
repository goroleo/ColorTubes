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
    explicit ColorsLayer(TubeItem *parent = 0, TubeModel *tm = 0);
    ~ColorsLayer();

    void paint(QPainter *painter) override;
    void setModel(TubeModel *tm);

public slots:
//    void fillColors(quint8 colorNum, quint8 count);
    void dropColors(quint8 count);

private slots:
    void onScaleChanged();
    void onAngleChanged();

private:
    TubeModel * m_model;
    TubeItem * parentTube;

    int      count();
    qreal    scale();
    qreal    angle();
    void     setAngle(qreal newAngle);

//  draw colors
    void     drawColors();
    QImage   * m_drawImage;
    QPainter * m_painter;

//  fill colors from another tube
    void     addFillArea(qreal fillAreaInc);
    QTimer * m_fillTimer;
    qreal    m_fillArea;                  // currently filled area
    qreal    m_fillAreaInc;               // area increment
    QColor   m_fillColor;                 // color to fill color cells
    quint8   m_fillCount;                 // number of filled color cells
    qreal    m_fillJetWidth;              // width of the jet
    QRectF   m_colorRect;                 // rect of the color
    qreal    m_colorBottom;               // bottom vertical coordinate

//  drop colors to another tube
    void     nextSegment();
    void     drawColorCell();

//    void addAngle(qreal angleInc);
//    QTimer * m_rotateTimer;
//    qreal    m_angleInc = 0.00001;                   // angle increment

    qreal  * m_tiltAngles;                 //
    qreal    m_startAngle;
    qreal    m_endAngle;
    quint8   m_dropCount;                  // number of dropped color cells

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

    PointF * tubeVertices;                      // coordinates after rotation
    LineF  * bottleLines;                       // bottle edge lines
    qreal    getIntersectionX(quint8 vertex);   // the intersection of the horizontal line from bottle vertices

    void     addSlice(qint8 vertex, qreal x1, qreal x2, qreal y);
    void     clearSlices();
    SliceF * tubeSlices;                         // tube is sliced by horizontal lines passing through its vertices
    qint8    m_slicesCount;
    qint8    m_sliceCurrent;

    void     addColorSegment(SliceF line);
    void     clearColorSegments();
    SliceF * colorSegments;                       // color segments coordinates
    qint8    m_segmentsCount;
    qint8    m_colorCurrent;
    qreal    m_segmentArea;

    SliceF   m_bottomLine;
    SliceF   m_topLine;

    // temporary
    qreal  * m_Areas;

};

#endif // COLORSLAYER_H
