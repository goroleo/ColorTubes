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
    quint8* m_colors;
    quint8  m_count;

//  draw static colors
    void drawColors();
    QImage*    m_drawImage;
    QPainter*  m_painter;

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

//  flow colors to another tube
    QTimer *m_rotateTimer;
    qreal  *m_tiltAngles;                 //
    qreal   m_angle;                      // current angle
    qreal   m_angleInc;                   // angle increment
    qreal   m_startAngle;
    qreal   m_endAngle;
    bool rotateLeft;

//  rotation coordinates

    struct PointF {
        qint8 pointNumber;
        qreal x;
        qreal y;
    };

    struct PointP {
        qint8 pointNumber;
        qreal r;    // polar radius from the rotation point
        qreal a;    // polar angle from the rotation point
    };

    struct SegmentCoo {
        qint8 pointNumber;
        qreal x1;   // right X
        qreal x2;   // left X
        qreal y;    // Y coordinate
    };


    PointF rotationPoint;                  // rotation point coordinate

    PointP* bottlePolarCoordinates;        // polar coordinates of the bottle vertices relative to the rotation point
    PointF* bottleCoordinates;             // coordinates after rotation
    QLineF* edgeLines;                     // bottle edges

    SegmentCoo* slicesCoordinates;         // tube is sliced by horizontal lines passing through its vertices
    qint8 m_slicesCount;
    qint8 sliceCurrent;
    qreal sliceArea;

    SegmentCoo* segmentsCoordinates;       // color segments coordinates
    qint8 segmentsCount;



    void addSlicePoints(qint8 pNumber, qreal x1, qreal x2, qreal y);
    void clearSlicesPoints();
    qreal getIntersection(quint8 point);    // the intersection of the horizontal line from bottle vertices



};

#endif // COLORSLAYER_H
