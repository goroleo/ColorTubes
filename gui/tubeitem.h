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
    Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
    Q_PROPERTY(int shade READ shade WRITE setShade NOTIFY shadeChanged)

    friend class ColorsLayer;  // the inner layer, it calculates vertical shift when rotate
    friend class BottleLayer;  // the inner layer, it uses vertical shift when rotate

public:
    explicit TubeItem(QQuickItem * parent = 0, TubeModel * tm = 0);
    ~TubeItem();

    TubeModel   * model() { return m_model; }

    void          setPosition(const QPointF newPoint);
    QPointF       position() {return m_regularPosition;}

    int           tubeIndex();
    bool          isClosed();
    bool          isEmpty();
    bool          isActive();
    bool          isFlyed();
    bool          isPouredIn();
    bool          isSelected();

    int           shade();
    void          setSelected(bool value);
    void          showAvailable(bool value);
    void          showClosed(bool value);

    quint8        currentColor();
    quint8        colorAt(quint8 index);
    bool          canPutColor(quint8 colorNumber);
    bool          canExtractColor();
    void          putColor(quint8 colorNumber);
    quint8        extractColor();

    void          moveColorTo(TubeItem * tube);

public slots:
    void          setShade(int newShade);

signals:
    void          angleChanged(const qreal newAngle);
    void          shadeChanged(const int newShade);

private slots:
    void          onScaleChanged();

private:
    GameBoard   * m_board;  // parent component
    TubeModel   * m_model;  // current model

    void          mousePressEvent(QMouseEvent * event);

//  layers
    CorkLayer   * m_cork;
    BottleLayer * m_front;
    ColorsLayer * m_colors;
    BottleLayer * m_back;
    ShadeLayer  * m_shade;

//  scale, position & rotation
    qreal         scale() const;

    void          setCurrentPosition(QPointF newPoint);
    void          setVerticalShift(qreal yShift);
    QPointF       m_regularPosition;
    QPointF       m_currentPosition;
    qreal         m_verticalShift;

    qreal         angle() const;
    void          setAngle(qreal newAngle);
    qreal         m_currentAngle    = 0.0;

//  animation frames
    void          startAnimation();         // ... with predefined (pre-calculated) parameters
    void          currentFrame();           // calculates current frame of animation

    QTimer      * m_timer;
    QPointF       m_startPoint;
    QPointF       m_endPoint;
    QPointF       m_moveIncrement;

    qreal         m_startAngle;
    qreal         m_endAngle;
    qreal         m_angleIncrement;

    static const int TIMER_TICKS    = 10;
    static const int STEPS_UP       = 3;
    static const int STEPS_DOWN     = 5;
    static const int STEPS_FLY      = 10;
    static const int STEPS_POUR_OUT = 24;
    static const int STEPS_FLYBACK  = 15;
    int           steps;

//  animation stages
    void          nextStage();              // calls the next animation' stage after this one has finished

    void          regularTube();
    void          moveUp();                  // show this tube as selected
    void          moveDown();                // deselect this tube
    void          flyTo(TubeItem * tubeTo);  // fly to pour colors out into specified tube (tubeTo)
    void          pourOut();                 //
    void          flyBack();                 // fly back to the original position

    static const int STAGE_REGULAR  = 0;
    static const int STAGE_SELECT   = 1;
    static const int STAGE_FLY_OUT  = 2;
    static const int STAGE_POUR_OUT = 3;
    static const int STAGE_FLY_BACK = 4;
    static const int STAGE_POUR_IN  = 10;
    int           currentStageId = 0;
    int           nextStageId = 0;

//  recipient tube variables/procedures
    TubeItem    * m_recipient = nullptr;

    void          connectTube(TubeItem * tubeFrom); // connects tubeFrom to this tube to pour colors out
    void          removeConnectedTube(TubeItem * tubeFrom);
    void          addPouringArea();
//  qreal         getPouringArea();         // the real area of poured colors. may be used in the future

    quint8        m_fillingColor;           // color number
    quint8        m_pouringCells = 0;       // number of pouring color cells
    quint8        m_connectedTubes = 0;     // number of tubes which will be pour colors into this tube
    quint8        m_pouredTubes = 0;        // number of tubes which are pours colors into this tube
    qreal         m_pouringArea = 0;        //

};

#endif // TUBEITEM_H
