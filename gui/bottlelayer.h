#ifndef BOTTLELAYER_H
#define BOTTLELAYER_H

#include <QObject>
#include <QQuickPaintedItem>
//#include <QPainter>
#include <QPixmap>

class BottleLayer : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal scale READ scale WRITE setScale NOTIFY scaleChanged)
//    Q_PROPERTY(bool visible READ visible )

public:
    explicit BottleLayer(QQuickItem *parent = 0);
    ~BottleLayer();

    void paint(QPainter *painter) override;

    qreal scale();
    bool visible();
    QString source();

public slots:
    void setSource(QString newSource);
    void setScale(qreal newScale);

signals:
    void scaleChanged(const qreal newScale);
    void sourceChanged(const int newShadeNumber);

private slots:
    void onScaleChanged();

private:
    void prepareImage();

    QString m_source;
    bool    m_visible = true;

    QPixmap  m_drawImage;

};

#endif // BOTTLELAYER_H
