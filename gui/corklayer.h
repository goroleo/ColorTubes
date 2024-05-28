#ifndef CORKLAYER_H
#define CORKLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QPixmap>

class CorkLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ isVisible  WRITE setVisible NOTIFY visibleChanged)

public:
    explicit CorkLayer(QQuickItem *parent = 0);
    ~CorkLayer();

    bool isVisible();

public slots:
    void setVisible(bool newVisible);
    void startShow();
    void startHide();

signals:
    bool visibleChanged(const bool newVisible);

private slots:
    void onScaleChanged();

private:

    void paint(QPainter *painter) override;

    QTimer *internalTimer;

    void nextStep();
    void prepareImage();

    QImage m_drawImage;
    bool   m_visible = false;
    qreal  m_alpha = 0.0;
    qreal  m_alphaIncrement;
    qreal  m_currentY = 0;
};

#endif // COLORSLAYER_H
