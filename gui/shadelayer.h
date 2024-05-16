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
    Q_PROPERTY(bool visible READ isVisible )
    Q_PROPERTY(bool pulse READ pulse WRITE setPulse NOTIFY pulseChanged)

public:
    explicit ShadeLayer(QQuickItem *parent = 0);
    ~ShadeLayer();

    void paint(QPainter *painter) override;

    qreal scale();
    int shade();
    bool isVisible();
    bool pulse();

    void startShow();
    void startHide();
    void hideImmediately();
    void startPulse();
    void stopPulse();

    void setShadeAfterHiding(int newShadeNumber);

public slots:
    void setShade(int newShadeNumber);
    void setPulse(bool value);

signals:
    void shadeChanged(const int newShadeNumber);
    void pulseChanged(const bool newPulse);

private slots:
    void onScaleChanged();

private:
    void nextAlpha();
    void prepareImage();

    quint8      m_shadeNumber = 0;
    quint8      m_shadeAfterHiding = 0;
    bool        m_visible = false;
    bool        m_pulse = false;

    qreal       m_alpha = 0;
    qreal       m_alphaIncrement;
    QTimer      *shadingTimer;

    QImage      m_shadeImage;
    QImage      m_drawImage;

};

#endif // SHADELAYER_H
