#ifndef TUBEITEM_H
#define TUBEITEM_H

#include <QQuickItem>

class CorkLayer;
class BottleLayer;
class ColorsLayer;
class ShadeLayer;
class TubeModel;
class GameBoard;

class TubeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(int shade READ shade WRITE setShade NOTIFY shadeChanged)

public:
    explicit TubeItem(QQuickItem *parent = 0, TubeModel * tm = 0);
    ~TubeItem();

    qreal scale() const;
    qreal angle() const;
    int shade();
    void rotate();
    TubeModel * model() {return m_model;}

    QPointF pivotPoint();

    void setPivotPoint(QPointF newPoint);
    void setYPrecision(qreal yp);

    qreal yPresision() {return m_yPrecision;}

    bool isCLosed();
    bool isEmpty();
    bool isPoured();
    bool isDischarged();

public slots:
    void setScale(qreal newScale);
    void setAngle(qreal newAngle);
    void setShade(int newShade);

signals:
    void scaleChanged(const qreal newScale);
    void angleChanged(const qreal newAngle);
    void shadeChanged(const int newShade);

private slots:
    void onScaleChanged();

private:
    GameBoard   * m_board;

    CorkLayer   * cork;
    BottleLayer * front;
    ColorsLayer * colors;
    BottleLayer * back;
    ShadeLayer  * m_shade;

    TubeModel   * m_model;

    qreal         m_angle = 0.0;
    qreal         m_angleIncrement;

    void mousePressEvent(QMouseEvent* event);
    void placeLayers();

    void addAngleIncrement();
    QTimer      * m_rotateTimer;

    QPointF      m_pivotPoint;

    bool         m_poured;
    bool         m_discharged;

    qreal        m_yPrecision;
};

#endif // TUBEITEM_H
