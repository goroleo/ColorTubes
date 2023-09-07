#ifndef GAME_H
#define GAME_H

class UsedColors;
class JctlFormat;
class BoardModel;
//class TubeFlyer;
//class ShadeLayer;
//class CtImageLayer;

/*!
 * \brief The Game singleton class
 */
class Game
{
public:
    static Game& create();
    static Game& instance();

    ~Game();

    UsedColors *usedColors;
    JctlFormat *jctl;
    BoardModel *board;

    void setMode(int newMode);
    int mode() {return gameMode;}

private:
    Game() {};
    void initialize();

//    TubeFlyer *flyer;
//    ShadeLayer *shadeLayer;
//    CtImageLayer *imageLayer;

    int gameMode;
    static Game* m_instance;
};


#endif // GAME_H
