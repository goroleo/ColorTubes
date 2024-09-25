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

    friend class ColorsLayer;  // the inner layer, it calculates vertical shift when rotate
    friend class BottleLayer;  // the inner layer, it uses vertical shift when rotate
    friend class GameBoard;

public:
    explicit TubeItem(QQuickItem * parent = 0, TubeModel * tm = 0);
    ~TubeItem();

    TubeModel   * model() const { return m_model; }

    bool          isDone();                        // unused?
    bool          isClosed() { return m_closed;}   // unused?
    bool          isEmpty();                       // unused?
    bool          isActive();
    bool          isFlyed();
    bool          isPouredIn();                    // unused?
    bool          isSelected();

    void          refresh();
    void          setSelected(bool value);
    void          showAvailable(bool value);
    void          setClosed(bool value);

    quint8        currentColor();
    bool          canPutColor(quint8 colorNumber);
    bool          canExtractColor();

    void          moveColorTo(TubeItem * tube);

signals:
    void          angleChanged(const qreal newAngle);

private slots:
    void          onScaleChanged();
    void          onTubeStateChanged();

private:
    GameBoard   * m_board;  // parent component
    TubeModel   * m_model;  // this tube's model

    void          mousePressEvent(QMouseEvent * event);

    int           tubeIndex();
    quint8        colorAt(quint8 index);
    void          setShade(int newShade);

//  layers from top to bottom
    CorkLayer   * m_cork;
    BottleLayer * m_front;
    ColorsLayer * m_colors;
    BottleLayer * m_back;
    ShadeLayer  * m_shade;

//  scale, position & rotation
    qreal         scale() const;                           // unused?

    QPointF       position() { return m_regularPosition;}  // unused?
    void          setRegularPosition(QPointF newPoint);
    void          setCurrentPosition(QPointF newPoint);
    void          setVerticalShift(qreal yShift);
    QPointF       m_regularPosition;
    QPointF       m_currentPosition;
    qreal         m_verticalShift;

    qreal         angle() const;
    void          setAngle(qreal newAngle);
    qreal         m_currentAngle    = 0.0;
    bool          m_closed;

//  animation frames
    void          startAnimation();         // ... with predefined (pre-calculated) parameters
    void          nextFrame();              // calculates current frame of animation

    QTimer      * m_timer;

    int           steps;
    QPointF       m_startPoint;
    QPointF       m_endPoint;
    QPointF       m_moveIncrement;
    qreal         m_startAngle;
    qreal         m_endAngle;
    qreal         m_angleIncrement;
    uint          m_endAngleNumber;         // used for pouring out rotation only

//  animation stages
    void          nextStage();              // calls the next animation' stage after this one has finished
    void          regularTube();            // tube at its regular position
    void          moveUp();                 // show tube as selected
    void          moveDown();               // deselect this tube
    void          flyTo(TubeItem * tubeTo); // fly to the specified tube to pour colors into it
    void          pourOut();                // pour colors out
    void          flyBack();                // fly back to the original position

    int           currentStageId = 0;
    int           nextStageId = 0;

//  recipient tube variables/procedures
    TubeItem    * m_recipient = nullptr;

    void          connectTube(TubeItem * tubeFrom); // connects tubeFrom to this tube to pour colors out
    void          removeConnectedTube(TubeItem * tubeFrom);
    void          addPouringArea();

    quint8        m_fillingColor;           // color number
    quint8        m_pouringCells = 0;       // number of pouring color cells
    quint8        m_connectedTubes = 0;     // number of tubes which will be pour colors into this tube
    quint8        m_pouredTubes = 0;        // number of tubes which are pours colors into this tube
};

class TubeItems: public QVector<TubeItem*>{};

#endif // TUBEITEM_H
