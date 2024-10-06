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
class Options;

/*!
 * \brief The Game singleton class
 */
class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool hasMoves READ hasMoves NOTIFY movesChanged)
    Q_PROPERTY(int level READ level NOTIFY levelChanged)

public:
    static Game & create();
    static Game & instance();
    ~Game();

    BoardModel * boardModel() { return m_board; }
    UsedColors * usedColors() { return m_usedColors; }
    JctlFormat * jctl()       { return m_jctl; }
    MoveItems  * moves()      { return m_moves; }

    MoveItem   * addNewMove(TubeModel &tubeFrom, TubeModel &tubeTo);
    bool         hasMoves();
    void         removeLastMove();

    void         setMode(int newMode);
    int          mode() {return m_gameMode;}
    quint32      level();

    bool         load(QString fileName);
    bool         loadTemporary();
    bool         save(QString fileName);
    bool         saveTemporary();
    void         removeTemporary();

    void         checkSolved();

    Q_INVOKABLE void newLevel();
    Q_INVOKABLE void undoMove();
    Q_INVOKABLE void startAgain();
    Q_INVOKABLE void solve();

signals:
    void         levelChanged();
    void         movesChanged();
    void         needToRefresh();
    void         solved();

private slots:
    void         onApplicationStateChanged();

private:
    explicit Game(QObject* parent = nullptr) : QObject(parent) {};
    void initialize();
    static Game * m_instance;

    UsedColors  * m_usedColors;
    JctlFormat  * m_jctl;
    BoardModel  * m_board;
    MoveItems   * m_moves;
    Options     * m_options;
    int           m_gameMode;
};


#endif // GAME_H
