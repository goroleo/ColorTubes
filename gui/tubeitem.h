#ifndef TUBEITEM_H
#define TUBEITEM_H

#include <QQuickPaintedItem>

class CorkLayer;
class BottleLayer;
class ColorsLayer;
class ShadeLayer;
class TubeModel;

class TubeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(int shade READ shade WRITE setShade NOTIFY shadeChanged)

public:
    explicit TubeItem(QQuickItem *parent = 0);
    ~TubeItem();

    qreal scale() const;
    qreal angle() const;
    int shade();
    void rotate();

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
    CorkLayer   * cork;
    BottleLayer * front;
    ColorsLayer * colors;
    BottleLayer * back;
    ShadeLayer  * m_shade;

    TubeModel   * model;

    qreal         m_angle = 0.0;
    qreal         m_angleIncrement;

    void mousePressEvent(QMouseEvent* event);
    void resize();

    void addAngle(qreal value);
    QTimer      * m_rotateTimer;


};

#endif // TUBEITEM_H
