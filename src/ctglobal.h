#ifndef CTGLOBAL_H
#define CTGLOBAL_H

#include <QtGlobal>
#include <QColor>

class Game;
class Io;
class TubeImages;
class Palette;
class BoardModel;

// ------------- radians & degrees
#define CT_PI          3.14159265358979323846              // PI
#define CT_2PI         6.28318530717958647692              // 2*PI
#define CT_PI2         1.57079632679489661923              // PI/2
#define CT_DEG2RAD     0.01745329251994329576922222222222  // PI/180
#define CT_RAD2DEG     57.295779513082320876846364344191   // 180/PI

// ------------- number of game colors
#define NUM_OF_COLORS       12               // don't forget to change palette!

// ------------- game modes
#define END_GAME            0
#define FILL_MODE           100
#define PLAY_MODE           200
#define ASSIST_MODE         300
#define BUSY_MODE           400

// ------------- tubes animation settings
#define TUBE_TIMER_TICKS    10
#define TUBE_STEPS_UP       3
#define TUBE_STEPS_DOWN     5
#define TUBE_STEPS_FLY      10
#define TUBE_STEPS_POUR_OUT 22
#define TUBE_STEPS_FLYBACK  15
#define CORK_ALPHA_INC      0.12
#define CORK_ALPHA_DEC     -0.18
#define SHADE_ALPHA_INC     0.065;
#define SHADE_ALPHA_DEC    -0.1;

// ------------- tubes stages
#define TUBE_STAGE_REGULAR  0
#define TUBE_STAGE_SELECT   1
#define TUBE_STAGE_FLY_OUT  2
#define TUBE_STAGE_POUR_OUT 3
#define TUBE_STAGE_FLY_BACK 4
#define TUBE_STAGE_POUR_IN  10

namespace CtGlobal
{
    void create();
    void destroy();

    // set of global singletons
    Game & game();
    Io & io();
    TubeImages & images();
    Palette & palette();

    // files
    QString localFile(QString fName);
    QString paletteFile();
    QString settingsFile();

    int    gameMode();
    qreal  scale();
    BoardModel  *  board();
    qreal  tubeWidth();
    qreal  tubeHeight();

    QColor paletteColor(quint8 colorIndex);

    // services
    quint32 colorStrToRgb(bool &ok, QString value);
    QString colorRgbToStr(quint32 value);
    QString intToStr(int value);
    QString intToHex(int value);
    QString endOfLine();
    template <typename T> int sgn(T val) {
        return (T(0) < val) - (val < T(0));
    }
}

#endif // CTGLOBAL_H
