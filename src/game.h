#ifndef GAME_H
#define GAME_H

#include <QObject>

class UsedColors;
class JctlFormat;
class BoardModel;
class TubeModel;
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

    MoveItem   * addNewMove(TubeModel &tubeFrom, TubeModel &tubeTo);
    bool         hasMoves();
    void         removeLastMove();

    void         setMode(int newMode);
    int          mode() {return gameMode;}

    bool         load(QString fileName);
    bool         loadTemporary();
    bool         save(QString fileName);
    bool         saveTemporary();
    void         removeTemporary();

signals:
    void         gameLoaded();

private:
    explicit Game() {};
    void initialize();

    UsedColors  * m_usedColors;
    JctlFormat  * m_jctl;
    BoardModel  * m_board;
    MoveItems   * m_moves;
    int           gameMode;
    static Game * m_instance;
};

#endif // GAME_H
