#ifndef CORKLAYER_H
#define CORKLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>

class CorkLayer : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit CorkLayer(QQuickItem * parent = 0);
    ~CorkLayer();

    bool     isVisible();

    void     setVisible(bool newVisible);
    void     startShow();
    void     startHide();

private slots:
    void     onScaleChanged();

private:
    void     paint(QPainter *painter) override;

    QTimer * m_timer;

    void     nextFrame();
    void     paintFrame();

    QImage   m_drawImage;
    bool     m_visible = false;
    qreal    m_alpha = 0.0;
    qreal    m_alphaIncrement;
    qreal    m_currentY = 0;
};

#endif // COLORSLAYER_H
