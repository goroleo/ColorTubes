#ifndef CTGLOBAL_H
#define CTGLOBAL_H

#include <QtGlobal>
#include <QColor>
#include <QtMath>

// ------------- radians & degrees
static const qreal   CT_PI2              = M_PI / 2.0;   //  90°
static const qreal   CT_PI               = M_PI;         // 180°
static const qreal   CT_3PI2             = M_PI * 1.5;   // 270°
static const qreal   CT_2PI              = M_PI * 2.0;   // 360°
static const qreal   CT_DEG2RAD          = M_PI / 180.0; // degrees to radians
static const qreal   CT_RAD2DEG          = 180.0 / M_PI; // radians to degrees

// ------------- number of colors in the game
static const quint8  CT_NUM_OF_COLORS    = 12;           // don't forget to change palette!

// ------------- game modes  //  reserved for future use
static const quint32 CT_END_GAME         = 0;
static const quint32 CT_FILL_MODE        = 100;
static const quint32 CT_PLAY_MODE        = 200;
static const quint32 CT_ASSIST_MODE      = 300;
static const quint32 CT_BUSY_MODE        = 400;

// ------------- tubes animation settings
static const quint8  CT_TIMER_TICKS      = 10;           // ticks interval in milliseconds
static const quint8  CT_TUBE_STEPS_UP    = 3;            // steps to move tube up
static const quint8  CT_TUBE_STEPS_DOWN  = 5;            // steps to move tube down
static const quint8  CT_TUBE_STEPS_FLY   = 10;           // steps to fly/move tube to another tube
static const quint8  CT_TUBE_STEPS_POUR  = 18;           // steps to pour out one color cell
static const quint8  CT_TUBE_STEPS_BACK  = 15;           // steps to fly/move tube back to its place
static const quint8  CT_SHADE_STEPS_INC  = 10;           // steps to opacity increment when shade layer appears
static const quint8  CT_SHADE_STEPS_DEC  = 15;           // steps to opacity decrement when shade layer disappears
static const quint8  CT_CORK_STEPS_INC   = 9;            // steps to opacity increment when cork layer appears
static const quint8  CT_CORK_STEPS_DEC   = 9;            // steps to opacity decrement when cork layer disappears

// ------------- tubes animation stages
static const quint8  CT_STAGE_DEFAULT    = 0;            // tube is at its regular position
static const quint8  CT_STAGE_SELECT     = 1;            // tube is selected
static const quint8  CT_STAGE_FLY        = 2;            // tube is flying to another tube to pour its color(s) into
static const quint8  CT_STAGE_POUR_OUT   = 3;            // tube pours its color(s) out
static const quint8  CT_STAGE_BACK       = 4;            // tube is flying back to its position
static const quint8  CT_STAGE_BUSY       = 10;           // tube is receiving colors from another tube

// ------------- tubes states
static const quint8  CT_STATE_EMPTY      = 0;            // there's no any filled color cell in the tube
static const quint8  CT_STATE_REGULAR    = 1;            // the tube has at least one empty cell to filling
static const quint8  CT_STATE_FILLED     = 2;            // all 4 color cells are filled with different colors
static const quint8  CT_STATE_DONE       = 3;            // all 4 color cells are filled with the same color

class Game;
class CtIo;
class CtImages;
class CtPalette;
class BoardModel;
class MoveItems;

namespace CtGlobal
{
    void create();
    void destroy();

    // global singletons
    Game & game();
    CtIo & io();
    CtImages & images();
    CtPalette & palette();

    // game
    BoardModel * board();
    MoveItems * moves();

    // files
    QString paletteFileName();
    QString settingsFileName();
    QString tempFileName();

    // colors
    QColor paletteColor(quint8 colorIndex);

    // service
    quint32 colorStrToRgb(bool &ok, QString value);
    QString colorRgbToStr(quint32 value);
    QString intToStr(int value);
    QString intToHex(int value);
    template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
    }
}

#endif // CTGLOBAL_H
