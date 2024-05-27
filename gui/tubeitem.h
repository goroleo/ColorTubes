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
    Q_PROPERTY(qreal angle READ angle NOTIFY angleChanged)
    Q_PROPERTY(int shade READ shade WRITE setShade NOTIFY shadeChanged)

    friend class ColorsLayer;  // the inner layer, it calculates vertical shift when rotate
    friend class BottleLayer;  // the inner layer, it uses vertical shift when rotate
    friend class GameBoard;

public:
    explicit TubeItem(QQuickItem * parent = 0, TubeModel * tm = 0);
    ~TubeItem();

    TubeModel   * model() { return m_model; }

    int           tubeIndex();

    void          setPosition(const QPointF newPoint);
    QPointF       position() {return m_regularPosition;}

    bool          isDone();
    bool          isClosed() {return m_closed;}
    bool          isEmpty();
    bool          isActive();
    bool          isFlyed();
    bool          isPouredIn();
    bool          isSelected();

    void          refresh();
    int           shade();
    void          setSelected(bool value);
    void          showAvailable(bool value);
    void          setClosed(bool value);

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

    bool          m_closed;


//  animation frames
    void          startAnimation();         // ... with predefined (pre-calculated) parameters
    void          currentFrame();           // calculates current frame of animation

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
