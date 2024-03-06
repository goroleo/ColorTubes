#include "flowlayer.h"

#include <QPainter>
#include <QBrush>
#include <QtMath>

#include "src/ctglobal.h"
#include "src/game.h"
#include "src/tubeimages.h"
#include "src/palette.h"
#include "core/tubemodel.h"
#include "gui/tubeitem.h"

FlowLayer::FlowLayer(TubeItem * parent, TubeModel * tm) :
    QQuickPaintedItem((QQuickItem *) parent),
    m_model(tm)
{
    parentTube = parent;

    m_drawImage = new QImage(80 * CtGlobal::images().scale(),
                             200 * CtGlobal::images().scale(),
                             QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    QObject::connect(&CtGlobal::images(), SIGNAL(scaleChanged(qreal)),
            this, SLOT(onScaleChanged()));

    m_fillTimer = new QTimer(this);
    connect(m_fillTimer, &QTimer::timeout, [=]() {
 //       addFillArea(m_fillAreaInc);
        update();
    });

}


FlowLayer::~FlowLayer()
{
    delete m_fillTimer;
    delete m_drawImage;
    delete m_painter;
}

void FlowLayer::paint(QPainter *painter)
{
    painter->drawImage(0, 0, * m_drawImage);
}

qreal FlowLayer::scale()
{
    return CtGlobal::images().scale();
}


void FlowLayer::onScaleChanged()
{
    if (m_drawImage) {
        delete m_painter;
        delete m_drawImage;
    }

    m_drawImage = new QImage(80 * scale(), 200 * scale(), QImage::Format_ARGB32);
    m_painter = new QPainter(m_drawImage);
    m_painter->setPen(Qt::NoPen);

    m_painter->fillRect(0,0,m_drawImage->width(), m_drawImage->height(), QColor(0x55FFFFFF));

    setWidth(80 * scale());
    setHeight(200 * scale());

//    drawColors();
    update();
}
