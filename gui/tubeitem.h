#ifndef TUBEITEM_H
#define TUBEITEM_H

#include <QQuickItem>

class CorkLayer;
class BottleLayer;
class ColorsLayer;
class ShadeLayer;
class FlowLayer;
class TubeModel;
class GameBoard;

class TubeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(int shade READ shade WRITE setShade NOTIFY shadeChanged)

public:
    explicit TubeItem(QQuickItem *parent = 0, TubeModel * tm = 0);
    ~TubeItem();

    qreal scale() const;
    qreal angle() const;
    int shade();
    TubeModel * model() { return m_model; }

    QPointF pivotPoint();

    void setPivotPoint(QPointF newPoint);
    void setYShift(qreal yp);

    qreal yShift() {return m_yShift;}

    bool isCLosed();
    bool isEmpty();
    bool isPoured();
    bool isDischarged();
    bool isSelected();

    void setSelected(bool newSelected);

    void rotate();
    void flyTo(TubeItem * tube);
    void flyBack();


public slots:
    void setAngle(qreal newAngle);
    void setShade(int newShade);

signals:
    void angleChanged(const qreal newAngle);
    void shadeChanged(const int newShade);

private slots:
    void onScaleChanged();

private:
    GameBoard   * m_board;
    TubeModel   * m_model;

    CorkLayer   * m_cork;
    BottleLayer * m_front;
    ColorsLayer * m_colors;
    BottleLayer * m_back;
    ShadeLayer  * m_shade;
    FlowLayer   * m_flow;


    void mousePressEvent(QMouseEvent* event);
    void placeLayers();

    void addAngleIncrement();

    bool          m_poured = false;
    bool          m_discharged = false;
    bool          m_selected = false;

    QPointF       m_pivotPoint;
    qreal         m_yShift;

    QTimer      * m_timer;
    QPointF       startPoint;
    QPointF       endPoint;
    QPointF       currentPoint;
    QPointF       moveIncrement;

    qreal         startAngle;
    qreal         endAngle;
    qreal         m_angle = 0.0;
    qreal         m_angleIncrement;

};

#endif // TUBEITEM_H
