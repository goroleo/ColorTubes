#ifndef COLORSLAYER_H
#define COLORSLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>
#include <QPainter>

class ColorsLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)

public:
    explicit ColorsLayer(QQuickItem *parent = 0);
    ~ColorsLayer();

    qreal scale();
    qreal angle();

    void paint(QPainter *painter) override;

public slots:
    void setScale(qreal newScale);
    void setAngle(qreal newAngle);
    void fillColors(quint8 colorNum, quint8 count);
    void dropColors(quint8 count);

signals:
    void scaleChanged(const qreal newScale);
    void angleChanged(const qreal newAngle);

private slots:
    void onScaleChanged();

private:
    quint8 *m_colors; // will be replaced by TubeModel
    quint8  m_count;  // will be replaced by TubeModel

//  draw colors
    void drawColors();
    QImage   *m_drawImage;
    QPainter *m_painter;

//  fill colors from another tube
    void addFillArea(qreal fillAreaInc);
    QTimer *m_fillTimer;
    qreal   m_fillArea;                  // currently filled area
    qreal   m_fillAreaInc;               // area increment
    QColor  m_fillColor;                 // color to fill color cells
    quint8  m_fillCount;                 // number of filled color cells
    qreal   m_fillJetWidth;              // width of the jet
    QRectF  m_colorRect;                 // rect of the color
    qreal   m_colorBottom;               // bottom vertical coordinate

//  drop colors to another tube
    void nextSegment();
    void drawColorCell();
    QTimer *m_rotateTimer;
    qreal  *m_tiltAngles;                 //
    qreal   m_angle;                      // current angle
    qreal   m_angleInc;                   // angle increment
    qreal   m_startAngle;
    qreal   m_endAngle;

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

    PointF *tubeVertices;                  // coordinates after rotation
    QLineF *edgeLines;                     // tube edges to calculate intersections
    qreal getIntersection(quint8 vertex);  // the intersection of the horizontal line from bottle vertices

    void addSlice(qint8 vertex, qreal x1, qreal x2, qreal y);
    void clearSlices();
    SliceF *tubeSlices;                    // tube is sliced by horizontal lines passing through its vertices
    qint8   m_slicesCount;
    qint8   m_sliceCurrent;
    qreal   m_sliceArea;

    void addColorSegment(qreal x1, qreal x2, qreal y);
    void addColorSegment(SliceF line);
    void clearColorSegments();
    SliceF *colorSegments;                 // color segments coordinates
    qint8   m_segmentsCount;
    qint8   m_colorCurrent;
    qreal   m_segmentArea;

    SliceF  m_bottomLine;
    SliceF  m_topLine;




};

#endif // COLORSLAYER_H
