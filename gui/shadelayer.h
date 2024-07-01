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

public:
    explicit ShadeLayer(QQuickItem *parent = 0);
    ~ShadeLayer();

    bool        isVisible();
    int         shade();
    bool        pulse();

    void        startShow();
    void        startHide();
    void        hideImmediately();
    void        startPulse();
    void        stopPulse();

    void        setShade(int newShadeNumber);
    void        setShadeAfterHide(int newShadeNumber);

    void        setPulse(bool value);

signals:
    void        pulseChanged(const bool newPulse);

private slots:
    void        onScaleChanged();

private:
    void        nextFrame();
    void        paintFrame();
    void        paint(QPainter * painter) override;

    quint8      m_shadeNumber = 0;
    quint8      m_shadeAfterHiding = 0;
    bool        m_visible = false;
    bool        m_pulse = false;

    qreal       m_alpha = 0;
    qreal       m_alphaIncrement;
    QTimer    * m_timer;

    QImage      m_shadeImage;
    QImage      m_drawImage;
};

#endif // SHADELAYER_H
