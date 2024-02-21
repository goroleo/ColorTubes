#ifndef CTGLOBAL_H
#define CTGLOBAL_H

#include <QtGlobal>

class Game;
class Io;
class TubeImages;
class Palette;

#define CT_PI          3.14159265358979323846              // PI
#define CT_2PI         6.28318530717958647692              // 2*PI
#define CT_PI2         1.57079632679489661923              // PI/2
#define CT_DEG2RAD     0.01745329251994329576922222222222  // PI/180
#define CT_RAD2DEG     57.295779513082320876846364344191   // 180/PI


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

    // game modes
    int    gameMode();

    static const int END_GAME    = 0;
    static const int FILL_MODE   = 100;
    static const int PLAY_MODE   = 200;
    static const int ASSIST_MODE = 300;
    static const int BUSY_MODE   = 400;

    // number of game colors
    static const int NUM_OF_COLORS = 12;

    // properties
//    void onScaleChanged();


    // services
    quint32 colorStrToRgb(bool &ok, QString value);
    QString colorRgbToStr(quint32 value);
    QString intToStr(int value);

}

#endif // CTGLOBAL_H
