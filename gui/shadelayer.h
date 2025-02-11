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
    bool        isBlinked();

    void        startShow();
    void        startHide();
    void        showImmediately();
    void        hideImmediately();
    void        startBlink();
    void        stopBlink();

    void        setShade(int newShadeNumber);
    void        setShadeAfterHide(int newShadeNumber);

    void        setBlinked(bool value);
    void        setAnimated(bool value);
    bool        isAnimated() {return m_animated;}

private slots:
    void        onScaleChanged();

private:
    void        nextFrame();
    void        paintFrame();
    void        paint(QPainter * painter) override;

    quint8      m_shadeNumber = 0;
    quint8      m_shadeAfterHiding = 0;
    bool        m_visible = false;
    bool        m_blinken = false;  // in the name of the Secretary of States )
    bool        m_blinkStopped = false;

    qreal       m_opacity = 0.0;
    qreal       m_opacityIncrement;
    QTimer    * m_timer;

    QImage      m_shadeImage;
    QImage      m_drawImage;
    bool        m_animated = false;
};

#endif // SHADELAYER_H
