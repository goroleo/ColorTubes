#ifndef GAME_H
#define GAME_H

#include <QObject>
class UsedColors;
class JctlFormat;
class BoardModel;
class GameBoard;
class MoveItems;
class MoveItem;

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

    BoardModel * boardModel() {return m_board; }
    UsedColors * usedColors() {return m_usedColors; }
    JctlFormat * jctl()       {return m_jctl; }
    MoveItems  * moves()      {return m_moves; }

    bool         hasMoves();
    void         clearMoves();
    MoveItem   * lastMove();
    void         deleteLastMove();

    void         setMode(int newMode);
    int          mode() {return gameMode;}

    void         load(QString fileName);
    void         save(QString fileName);

    MoveItem * addNewMove(int tubeFromIndex, int tubeToIndex);

signals:
    void onGameLoaded();

private:
    Game() {};
    void initialize();

    UsedColors  * m_usedColors;
    JctlFormat  * m_jctl;
    BoardModel  * m_board;
    MoveItems   * m_moves;
    int gameMode;
    static Game * m_instance;
};


#endif // GAME_H
