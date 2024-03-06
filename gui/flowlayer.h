#ifndef FLOWLAYER_H
#define FLOWLAYER_H

#include <QObject>
#include <QQuickPaintedItem>
#include <QTimer>
#include <QImage>
#include <QPainter>

class TubeModel;
class TubeItem;

class FlowLayer : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit FlowLayer(TubeItem *parent = 0, TubeModel *tm = 0);
    ~FlowLayer();

    void paint(QPainter *painter) override;


private slots:
    void onScaleChanged();

private:
    QImage * m_drawImage;
    TubeItem * parentTube;
    TubeModel * m_model;
    QPainter * m_painter;
    QTimer * m_fillTimer;

    qreal scale();

};

#endif // FLOWLAYER_H
