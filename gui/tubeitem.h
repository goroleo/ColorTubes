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

public:
    explicit TubeItem(QQuickItem *parent = 0);
    ~TubeItem();

    qreal scale() const;
    qreal angle() const;

public slots:
    void setScale(qreal newScale);
    void setAngle(qreal newAngle);

signals:
    void scaleChanged(const qreal newScale);
    void angleChanged(const qreal newAngle);

private slots:
    void onScaleChanged();

private:
    CorkLayer   *cork;
    BottleLayer *front;
    ColorsLayer *colors;
    BottleLayer *back;
    ShadeLayer  *shade;

    TubeModel *model;

    qreal        m_angle;

};

#endif // TUBEITEM_H
