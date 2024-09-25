#ifndef CTIMAGES_H
#define CTIMAGES_H

#include <QtSvg/QSvgRenderer>
#include <QPixmap>

// Qt is not supported SVG transparency masks. So we have to render them manually.
class CtImages : public QObject
{
    Q_OBJECT
public:
    static CtImages & create();
    static CtImages & instance();

    ~CtImages();

    qreal     scale()       {return m_scale;}

// scaled images of the tube
    QPixmap   bottle()      {return *m_bottle;}
    QPixmap   bottleBack()  {return *m_bottleBack;}
    QPixmap   bottleFront() {return *m_bottleFront;}

    QPixmap   shadeYellow() {return *m_shadeYellow;}
    QPixmap   shadeGreen()  {return *m_shadeGreen;}
    QPixmap   shadeBlue()   {return *m_shadeBlue;}
    QPixmap   shadeRed()    {return *m_shadeRed;}
    QPixmap   shadeGray()   {return *m_shadeGray;}

    QPixmap   cork()        {return *m_cork;}

// scaled tube's coordinates and sizes
    QPointF   vertex(quint8 index) {return m_vertices[index];}

    qreal     tubeWidth()   {return m_tubeWidth;}
    qreal     tubeFullWidth() {return m_tubeFullWidth;}
    qreal     tubeRotationWidth() {return m_tubeRotationWidth;}
    qreal     shiftWidth()  {return m_shiftWidth;}
    qreal     tubeHeight()  {return m_tubeHeight;}
    qreal     tubeFullHeight() {return m_tubeFullHeight;}
    qreal     shiftHeight() {return m_shiftHeight;}

    QRectF    colorRect(quint8 index);
    qreal     colorWidth()  {return m_colorWidth;}
    qreal     colorHeight() {return m_colorHeight;}
    qreal     colorArea()   {return m_colorArea;}

    qreal     jetWidth()    {return m_jetWidth;}
    qreal     jetHeight(quint8 index)
                 {return m_vertices[3].y() - m_colorHeight * index;}
    QRectF    jetRect()     {return m_jetRect;}

// angles to rotate tube
    qreal     tiltAngle(uint index);

public slots:
    void      setScale(qreal value);

signals:
    void      scaleChanged(qreal newScale);

private:
    explicit  CtImages(QObject *parent = nullptr);

    void      initialize();
    static    CtImages * m_instance;

    void      scaleVertices();
    QRectF    scaleRect (QRectF rect);
    void      renderImages();

    qreal     m_scale = 1.0;
    QSvgRenderer * m_source;

    QPixmap   * m_bottle;
    QPixmap   * m_bottleBack;
    QPixmap   * m_bottleFront;
    QPixmap   * m_shadeYellow;
    QPixmap   * m_shadeGreen;
    QPixmap   * m_shadeBlue;
    QPixmap   * m_shadeRed;
    QPixmap   * m_shadeGray;
    QPixmap   * m_cork;

    QPointF   * m_vertices;   // vertices of the tube

    qreal       m_tubeWidth;
    qreal       m_tubeFullWidth;
    qreal       m_tubeRotationWidth;
    qreal       m_shiftWidth;
    qreal       m_tubeHeight;
    qreal       m_tubeFullHeight;
    qreal       m_shiftHeight;

    qreal       m_colorWidth;  // width of the one color cell after scaling
    qreal       m_colorHeight;
    qreal       m_colorArea;   // colorWidth * colorHeight
    qreal       m_jetWidth;
    QRectF      m_jetRect;

    qreal     * m_tiltAngles;
    bool        anglesExist = false;
    void        calculateTiltAngles();
    bool        loadTiltAngles();
    bool        saveTiltAngles();

    qreal       lineLength(QPointF p1, QPointF p2);
    qreal       lineAngle(QPointF p1, QPointF p2);
    qreal       triangleArea(qreal lineLength, qreal angle1, qreal angle2);
};

#endif // CTIMAGES_H
