#ifndef SHADELAYER_H
#define SHADELAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QTimer>
#include <QImage>

class ShadeLayer : public QQuickPaintedItem
{

    Q_OBJECT
    Q_PROPERTY(int shade READ shade WRITE setShade NOTIFY shadeChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool visible READ visible )
    Q_PROPERTY(bool pulse READ pulse WRITE setPulse NOTIFY pulseChanged)
//    Q_PROPERTY(int width READ width WRITE setWidth)
//    Q_PROPERTY(int height READ height WRITE setHeight)

public:
    explicit ShadeLayer(QQuickItem *parent = 0);
    ~ShadeLayer();

    void paint(QPainter *painter) override;

    qreal scale();
    int shade();
    bool visible();
    bool pulse();

public slots:
    void setShade(int newShadeNumber);
    void setScale(qreal newScale);
    void setPulse(bool newPulse);
    void startShow();
    void startHide();
    void startPulse();
    void stopPulse();

signals:
    void scaleChanged(const qreal newScale);
    void shadeChanged(const int newShadeNumber);
    void pulseChanged(const bool newPulse);

private:
    void nextAlpha();
    void prepareImage();

    quint8      m_shadeNumber;
//    qreal       m_scale;
    bool        m_visible = false;
    bool        m_pulse = false;

    qreal       m_alpha = 0;
    qreal       m_alphaIncrement;
    QTimer      *internalTimer;

    QImage      m_shadeImage;
    QImage      m_drawImage;

    const float ALPHA_INC_UP = 0.03;
    const float ALPHA_INC_DOWN = -0.06;
    const int   TIMER_TICKS = 5;
    const int   TIMER_PULSE_TICKS = 9;

private slots:
    void onScaleChanged();

};

#endif // SHADELAYER_H
