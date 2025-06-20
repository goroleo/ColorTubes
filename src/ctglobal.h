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
static const quint8  CT_NUMBER_OF_COLORS = 13;           // don't forget to change palette!

// ------------- game modes  //  PLAY and ASSIST modes are using in this app only
// static const quint32 CT_END_GAME      = 0;
static const quint32 CT_FILL_MODE        = 100;          // unsupported here. It was used at PC version when MANUAL FILL was realized
static const quint32 CT_PLAY_MODE        = 200;
static const quint32 CT_ASSIST_MODE      = 300;
// static const quint32 CT_BUSY_MODE     = 400;          // use GameBoard::busy() instead

// ------------- tubes animation settings
static const quint8  CT_TIMER_TICKS      = 10;           // ticks interval in milliseconds
static const quint8  CT_BLINK_TICKS      = 30;           // timer ticks when blinking
static const quint8  CT_TUBE_STEPS_UP    = 3;            // steps to move tube up
static const quint8  CT_TUBE_STEPS_DOWN  = 5;            // steps to move tube down
static const quint8  CT_TUBE_STEPS_FLY   = 10;           // steps to fly/move tube to another tube
static const quint8  CT_TUBE_STEPS_POUR  = 18;           // steps to pour out one color cell
static const quint8  CT_TUBE_STEPS_BACK  = 10;           // steps to fly/move tube back to its place
static const quint8  CT_SHADE_STEPS_INC  = 20;           // steps to opacity increment when shade layer appears
static const quint8  CT_SHADE_STEPS_DEC  = 15;           // steps to opacity decrement when shade layer disappears
static const quint8  CT_CORK_STEPS_INC   = 10;           // steps to opacity increment when cork layer appears
static const quint8  CT_CORK_STEPS_DEC   = 8;            // steps to opacity decrement when cork layer disappears
static const quint8  CT_ARROW_STEPS_INC  = 6;            // steps to opacity increment when arrow appears
static const quint8  CT_ARROW_STEPS_DEC  = 5;            // steps to opacity decrement when arrow disappears

// ------------- flower (CongratsPanel) animation settings
static const quint8  CT_FLOWER_TICKS     = 10;               // ticks when flower rotates
static const qreal   CT_FLOWER_ANGLE_INC = 2.0 * CT_DEG2RAD; // flower angle rotation per one tick

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

// ------------- game solution results
static const quint8  CT_SOLVER_SUCCESS   = 0;            // the game solution is found successfully
static const quint8  CT_SOLVER_NOT_FOUND = 1;            // the game solution is not found
static const quint8  CT_SOLVER_NO_BOARD  = 2;            // start board is undefined, nothing to do
static const quint8  CT_SOLVER_NO_MOVES  = 3;            // start board has no any moves, nothing to do
static const quint8  CT_SOLVER_CANCELLED = 4;            // solution process has interrupted by user

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
    Game       & game();
    CtIo       & io();
    CtImages   & images();
    CtPalette  & palette();

    // game
    BoardModel * board();
    MoveItems  * moves();
    quint32      gameMode();

    // files
    QString      paletteFileName();
    QString      settingsFileName();
    QString      tempFileName();

    // colors
    QColor       paletteColor(quint8 colorIndex);
    bool         isLightTheme();

    // service
    quint32      crc32Table(int index);
    quint32      colorStrToRgb(bool &ok, QString value);
    QString      colorRgbToStr(quint32 value);
    QString      intToStr(int value);
    QString      intToHex(int value);
    template <typename T> int sign(T val) {
        return (T(0) < val) - (val < T(0));
    }
}

#endif // CTGLOBAL_H
