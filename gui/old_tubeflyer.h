#ifndef OLD_TUBEFLYER_H
#define OLD_TUBEFLYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QTimer>
#include <QImage>

class TubeModel;

class old_TubeFlyer : public QQuickPaintedItem
{

    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
//    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

public:
    explicit old_TubeFlyer(QQuickItem *parent);
    ~old_TubeFlyer();

    void setRotationPoint(int number);
    void setTube(TubeModel &tm);
    qreal scale();

//    int width() { return m_width; }
//    int height() { return m_height; }

    void paint(QPainter *painter) override;

public slots:
    void setScale(qreal newScale);

signals:
    void scaleChanged(const qreal newScale);
//    void visibleChanged(const bool newVisible);


private:
    #define MAX_SEGMENT_POINTS 6

    void rotateBottlePoints();

    void addSlicePoints(qint8 pNumber, qreal x1, qreal x2, qreal y);
    void clearSlicesPoints();
    void addColorPoints(qreal x1, qreal x2, qreal y);
    void clearColorsPoints();

    qreal getIntersection(quint8 point);    // the intersection of the horizontal line from bottle vertices

    void drawColors();
    void nextSegment();

    void drawColorCell();
    void drawFlow(qreal square);
    void drawBottle();

    struct PointF {
        qint8 pointNumber;
        qreal x;
        qreal y;
    };

    struct PointP {
        qreal r; // polar radius from the rotation point
        qreal a; // polar angle from the rotation point
    };

    struct SliceCoo {
        qint8 pointNumber;
        qreal x1; // right X
        qreal x2; // left X
        qreal y;  // Y coordinate
    };

    struct SegmentCoo {
        qreal x1; // right X
        qreal x2; // left X
        qreal y;  // Y coordinate
    };

    TubeModel *m_tube;

    qint16 m_width = 280;
    qint16 m_height = 200;
    qint16 vTranslate = 0; // moves picture vertically
    qint16 hTranslate = 0; // moves picture horizontally

    PointF rotationPoint;              // rotation point coordinates

    PointP* bottlePolarCoordinates;    // polar coordinates of the bottle vertices relative to the rotation point
    PointF* bottleCoordinates;         // coordinates after rotation

    SliceCoo* slicesCoordinates;       // tube is sliced by horizontal lines passing through its vertices
    qint8 slicesCount;
    qint8 sliceCurrent;
    qreal sliceArea;

    SegmentCoo* segmentsCoordinates;   // color segments coordinates
    qint8 segmentsCount;

    qint8 colorCurrent;
    qreal colorArea;

    QLineF* edgeLines;                 // bottle edges

    double angle;                      // current angle
    qreal delta;                       // current angle
    qreal startAngle;
    qreal endAngle;
    bool rotateLeft;

    qreal* tiltAngles;                 // values of the tilt angles at which each color cell is emptied

    bool bottleDrawn = false;

    SegmentCoo m_topLine;
    SegmentCoo m_bottomLine;

    QTimer      *internalTimer;

    QPainter    m_painter;
    QImage      m_drawImage;

    const float ALPHA_INC_UP = 0.03;
    const float ALPHA_INC_DOWN = -0.06;
    const int   TIMER_TICKS = 5;
    const int   TIMER_PULSE_TICKS = 9;


};

#endif // OLD_TUBEFLYER_H
