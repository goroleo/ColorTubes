#ifndef TUBEITEM_H
#define TUBEITEM_H

#include <QQuickPaintedItem>

class CorkLayer;
class BottleLayer;
class ColorsLayer;
class ShadeLayer;
class TubeModel;


class TubeItem : public QQuickPaintedItem
{

    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)

public:
    explicit TubeItem(QQuickItem *parent = 0);
    ~TubeItem();

    void paint(QPainter *painter) override;


    qreal scale() const;
    void setScale(qreal newScale);
    qreal angle() const;
    void setAngle(qreal newAngle);


private:
    CorkLayer   *cork;
    BottleLayer *front;
    ColorsLayer *colors;
    BottleLayer *back;
    ShadeLayer  *shade;

    TubeModel *model;

    QRectF clipRect() const override;

};

#endif // TUBEITEM_H
