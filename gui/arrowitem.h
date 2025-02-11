#ifndef ARROWITEM_H
#define ARROWITEM_H

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QPainter>
#include <QTimer>
#include <QImage>
#include "tubeitem.h"

class ArrowItem: public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit ArrowItem(QQuickItem *parent = 0);
    ~ArrowItem();

    quint8   arrowId() { return m_arrowId; }
    void     setArrowId(quint8 arrowId);

    static const quint8 CT_ARROW_NONE  = 0;
    static const quint8 CT_ARROW_OUT   = 1;
    static const quint8 CT_ARROW_IN    = 2;

    bool     isVisible() { return m_visible; }
    void     setVisible(bool value);
    void     startShow();
    void     startHide();
    void     hideImmediately();
    void     startBlink();
    void     stopBlink();
    void     setConnectedTube(TubeItem * tube);
    TubeItem * connectedTube() {return m_connectedTube;}

signals:
    void     shown();

private slots:
    void     onScaleChanged();
//    void     onShownChanged();

private:
    quint8   m_arrowId = CT_ARROW_NONE;

    void     paint(QPainter *painter) override;
    void     nextBodyFrame();
    void     nextStrokeFrame();
    void     paintBody();
    void     paintStroke();

    QImage   m_bodyImage;
    QImage   m_strokeImage;
    QImage   m_drawBodyImage;
    QImage   m_drawStrokeImage;

    QTimer * m_bodyTimer;
    QTimer * m_strokeTimer;
    bool     m_visible = false;
    qreal    m_bodyOpacity = 0.0;
    qreal    m_opacityIncrement = 0.0;

    qreal    m_startY;
    qreal    m_endY;
    qreal    m_incrementY;
    qreal    m_currentY;

    bool     m_blinken = false;
    qreal    m_strokeOpacity = 0.0;
    TubeItem * m_connectedTube = nullptr;
};

#endif // ARROWITEM_H
