#ifndef CORKLAYER_H
#define CORKLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QPixmap>

class CorkLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(bool visible READ visible  WRITE setVisible NOTIFY visibleChanged)

public:
    explicit CorkLayer(QQuickItem *parent = 0);
    ~CorkLayer();

    qreal scale();
    bool visible();

    void paint(QPainter *painter) override;

public slots:
    void setScale(qreal newScale);
    void setVisible(bool newVisible);
    void startShow();
    void startHide();


signals:
    void scaleChanged(const qreal newScale);
    bool visibleChanged(const bool newVisible);

private slots:
    void onScaleChanged();

private:

    QTimer *internalTimer;

    void nextStep();
    void prepareImage();

    QImage m_drawImage;
    bool m_visible;
    qreal m_alpha;
    qreal m_alphaIncrement;
    qreal m_currY;

    const float ALPHA_INC_UP = 0.08;
    const float ALPHA_INC_DOWN = -0.12;
    const int   PATH_SIZE = 20;
    const int   TIMER_TICKS = 5;

};

#endif // COLORSLAYER_H
