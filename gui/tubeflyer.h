#ifndef TUBEFLYER_H
#define TUBEFLYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QTimer>
#include <QImage>

class TubeFlyer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
//    Q_PROPERTY(int width READ width WRITE setWidth)
//    Q_PROPERTY(int height READ height WRITE setHeight)


public:
    explicit TubeFlyer(QQuickItem *parent = 0);
    ~TubeFlyer();

    void paint(QPainter *painter) override;

    qreal scale();
    qreal angle();

public slots:
    void setScale(qreal newScale);
    void setAngle(qreal newAngle);


signals:
    void scaleChanged(const qreal newScale);
    void angleChanged(const qreal newAngle);

private:
    void nextAngle();
    void calculatePoints();

    qreal       m_angle = 0;
    qreal       m_angleIncrement;
    qreal       m_startAngle;
    qreal       m_endAngle;

    QTimer      *internalTimer;

    QImage      m_bottleImage;
    QImage      m_drawImage;

    const int   TIMER_TICKS = 5;

// ~~~~~~~~~






};

#endif // TUBEFLYER_H
