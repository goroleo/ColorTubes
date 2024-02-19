#ifndef GAME_H
#define GAME_H

#include <QObject>
class UsedColors;
class JctlFormat;
class BoardModel;
class GameBoard;

/*!
 * \brief The Game singleton class
 */
class Game : public QObject
{
    Q_OBJECT
public:
    static Game & create();
    static Game & instance();

    ~Game();

    UsedColors *usedColors;
    JctlFormat *jctl;
//    GameBoard *board;

    void setMode(int newMode);
    int mode() {return gameMode;}

private:
    Game() {};
    void initialize();

    int gameMode;
    static Game* m_instance;
};


#endif // GAME_H
