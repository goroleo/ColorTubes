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

    void    setPosition(const QPointF newPoint);
    void    setPivotPoint(QPointF newPoint);
    QPointF pivotPoint();

    void setVerticalShift(qreal yp);
    qreal verticalShift() {return m_verticalShift;}

    bool isClosed();
    bool isEmpty();
    bool isActive();
    bool isPoured();
    bool isTilted();
    bool isSelected();

    bool canPutColor(quint8 colorNumber);
    bool canExtractColor();
    quint8 currentColor();

    int tubeIndex();

    void setSelected(bool value);

    void dropColorsTo(TubeItem * tube);

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

    TubeItem    * m_recipient = nullptr;

    void mousePressEvent(QMouseEvent* event);
    void placeLayers();

//    void addAngleIncrement();

    void startMove();
    void nextFrame();

// moving & animation stages
    void nextStage();
    void moveUp();
    void moveDown();
    void flyTo(TubeItem * tube);
    void flyBack();
    void discharge();

    QPointF       m_positionPoint;
    QPointF       m_pivotPoint;
    qreal         m_verticalShift;

    QTimer      * m_timer;
    QPointF       m_startPoint;
    QPointF       m_endPoint;
    QPointF       m_currentPoint;
    QPointF       m_moveIncrement;

    qreal         m_startAngle;
    qreal         m_endAngle;
    qreal         m_angle = 0.0;
    qreal         m_angleIncrement;
    quint8        m_dropColors;         // number of dropped color cells
    quint8        m_flowingTubes;       // number of tubes which are flew colors to this tube

    static const int STAGE_DEFAULT = 0;
    static const int STAGE_SELECT = 1;
    static const int STAGE_FLY = 2;
    static const int STAGE_DISCHARGE = 3;
    static const int STAGE_RETURN = 4;
    static const int STAGE_POUR = 10;
    int           currentStageId = 0;
    int           nextStageId = 0;

    static const int TIMER_TICKS = 10;
    static const int STEPS_UP = 5;
    static const int STEPS_DOWN = 3;
    static const int STEPS_FLY = 20;
    static const int STEPS_DISCHARGE = 26;
    static const int STEPS_FLYBACK = 15;
    int           steps;

    quint8 extractColor();
    void putColor(quint8 colorNum);

    quint8        m_fillColor;

};


#endif // TUBEITEM_H
