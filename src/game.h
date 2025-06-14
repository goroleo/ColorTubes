#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QtQuick>

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
    Q_PROPERTY(int  level        READ level        NOTIFY levelChanged)
    Q_PROPERTY(bool movesMade    READ movesMade    NOTIFY movesChanged)
    Q_PROPERTY(bool isAssistMode READ isAssistMode NOTIFY modeChanged)
    Q_PROPERTY(bool isLightTheme READ isLightTheme WRITE setLightTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isSystemTheme READ isSystemTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isSystemOrientation READ isSystemOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool isPortraitOrientation READ isPortraitOrientation NOTIFY orientationChanged)
    Q_PROPERTY(bool isUndoUntilSolvable READ isUndoUntilSolvable NOTIFY undoModeChanged)

public:
    static Game & create();
    static Game & instance();
    ~Game();

    BoardModel * boardModel() { return m_board; }
    UsedColors * usedColors() { return m_usedColors; }
    JctlFormat * jctl()       { return m_jctl; }
    MoveItems  * moves()      { return m_moves; }

    bool         load(QString fileName);
    bool         loadTemporary();
    bool         save(QString fileName);
    bool         saveTemporary();
    void         removeTemporary();

    quint32      mode() { return m_gameMode; }
    bool         isAssistMode();
    quint32      level();

    void         checkSolved();

    void         addMove(MoveItem * move);
    bool         movesMade();
    void         removeLastMove();
    MoveItem   * currentMove();
    MoveItem   * nextMove();

    Q_INVOKABLE void newLevel();
    Q_INVOKABLE void undoMove();
    Q_INVOKABLE void undoUntilSolvable();
    Q_INVOKABLE void startAgain();
    Q_INVOKABLE void solve();
    Q_INVOKABLE void startAssistMode();
    Q_INVOKABLE void endAssistMode();
    Q_INVOKABLE void resetGameplay();

    void         setLightTheme(bool value);
    bool         isLightTheme();
    bool         isSystemTheme();
    Q_INVOKABLE void setTheme(int mode);

    bool         isSystemOrientation();
    bool         isPortraitOrientation();
    Q_INVOKABLE void setOrientation(int mode);

    bool         isUndoUntilSolvable();
    Q_INVOKABLE void setUndoMode(int mode);

signals:
    void         themeChanged();
    void         orientationChanged();
    void         levelChanged();
    void         movesChanged();
    void         modeChanged();
    void         undoModeChanged();
    void         refreshNeeded();
    void         levelDone();

    void         solverSuccess();
    void         solverError();
    void         assistModeError();

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
    quint32       m_gameMode;
    quint16       m_movesDone;
    bool          m_lightTheme = false;
    bool          m_systemTheme = false;
};

#endif // GAME_H
