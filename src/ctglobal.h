#ifndef CTGLOBAL_H
#define CTGLOBAL_H

#include <QtGlobal>

class Game;
class Io;
class TubeImages;
class Palette;

namespace CtGlobal
{
    void create();
    void destroy();

    // set of global singletons
    Game& game();
    Io& io();
    TubeImages& images();
    Palette& palette();

    // files
    QString localFile(QString fName);
    QString paletteFile();
    QString settingsFile();



    // game modes
    void   setGameMode(int newMode);
    int    gameMode();

    static const int END_GAME    = 0;
    static const int FILL_MODE   = 100;
    static const int PLAY_MODE   = 200;
    static const int ASSIST_MODE = 300;
    static const int BUSY_MODE   = 400;

    // number of game colors
    static const int NUM_OF_COLORS = 12;

    // services
    quint32 colorStrToRgb(bool &ok, QString value);
    QString colorRgbToStr(quint32 value);
    QString intToStr(int value);

}

#endif // CTGLOBAL_H
