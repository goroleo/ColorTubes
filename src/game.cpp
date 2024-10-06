#include "game.h"
#include <qguiapplication.h>

#include "src/ctglobal.h"
#include "src/ctio.h"
#include "core/solver.h"
#include "core/jctlformat.h"
#include "core/options.h"
#include "core/boardmodel.h"
#include "core/moveitem.h"
#include "core/usedcolors.h"

Game * Game::m_instance = nullptr;

Game::~Game()
{
    if (!m_board->isSolved())
        saveTemporary();
    m_options->level = m_board->level();

    delete m_usedColors;
    delete m_jctl;
    delete m_options;
    delete m_board;

    m_moves->clear();
    delete m_moves;

    CtGlobal::destroy();

    m_instance = nullptr;
    qDebug() << "Game destroyed";
}

Game& Game::create()
{
    return instance();
}

Game& Game::instance()
{
    if (m_instance == nullptr) {
        qDebug() << "Creating game.";
        m_instance = new Game();
        CtGlobal::create();
        m_instance->initialize();
    }
    return *m_instance;
}

void Game::initialize()
{
    m_usedColors = new UsedColors;
    m_options = new Options;
    m_jctl = new JctlFormat;
    m_moves = new MoveItems;
    m_board = new BoardModel;

    QObject::connect(qGuiApp, &QGuiApplication::applicationStateChanged,
            this, &Game::onApplicationStateChanged);

    bool loaded = false;
    if (CtGlobal::io().tempFileExists())
        loaded = loadTemporary();

    if (!loaded) {
        newLevel();
    }
}

quint32 Game::level()
{
    return m_board->level();
}

void Game::newLevel()
{
    if (m_board->tubesCount() > 0) {
        m_options->level = m_board->level();
        m_options->save();
    }
    qDebug () << "Previous level:" << m_options->level;
    quint32 newLevel = m_options->level + 1;

    int filledTubes;
    int emptyTubes;

    if (newLevel < 3) {
        filledTubes = 2;
        emptyTubes = 1;
    } else if (newLevel < 5) {
        filledTubes = 3;
        emptyTubes = 1;
    } else if (newLevel < 10) {
        filledTubes = 5;
        emptyTubes = 2;
    } else if (newLevel < 20) {
        filledTubes = 7;
        emptyTubes = 2;
    } else if (newLevel < 50) {
        filledTubes = 9;
        emptyTubes = 2;
    } else {
        if (newLevel & 1) {
            filledTubes = 9;
            emptyTubes = 2;
        } else {
            filledTubes = 12;
            emptyTubes = 2;
        }
    }

    m_board->randomFill(filledTubes, emptyTubes);
    m_board->setLevel(newLevel);
    emit levelChanged();
    m_moves->clear();
    emit movesChanged();
}

bool Game::load(QString fileName)
{
    QByteArray buffer;
    bool result;

    result = CtGlobal::io().loadGame(fileName, buffer);

    if (result)
        result = m_jctl->read(buffer);

    if (result) {
        m_jctl->restoreGame(m_board);
        m_jctl->restoreMoves(m_moves);
    }

    if (result) {
        qDebug() << "Game loaded from" << fileName;
        // signal to redraw GameBoard
        emit levelChanged();
        emit movesChanged();
    } else
        qDebug() << "Game is not loaded." << fileName;

    return result;
}

bool Game::loadTemporary()
{
    return load(CtGlobal::tempFileName());
}

bool Game::save(QString fileName)
{
    QByteArray buffer;
    m_gameMode = CT_PLAY_MODE;
    m_jctl->storeGame();
    m_jctl->storeMoves();
    m_jctl->write(buffer, 2);
    return CtGlobal::io().saveGame(fileName, buffer);
}

bool Game::saveTemporary()
{
    return save(CtGlobal::tempFileName());
}

void Game::removeTemporary()
{
    CtGlobal::io().tempFileDelete();
}

bool Game::hasMoves()
{
    return !m_moves->empty();
}

void Game::removeLastMove()
{
    m_moves->removeLast();
}

MoveItem * Game::addNewMove(TubeModel &tubeFrom, TubeModel &tubeTo)
{
    MoveItem * move = new MoveItem(m_board->getMoveData(tubeFrom, tubeTo));
    m_moves->append(move);
    return move;
}

void Game::checkSolved()
{
    if (m_board->isSolved()) {
        qDebug() << "!!! SOLVED !!!";
        removeTemporary();
        emit solved();
    }
}

void Game::solve()
{
    saveTemporary();
    SolveProcess solver;
    solver.start(m_board);
}

void Game::undoMove()
{
    MoveItem * move = moves()->current();
    if (move) {
        for (int i = 0; i < move->count(); ++i) {
            m_board->tubeAt( move->tubeFrom() )->putColor(
                        m_board->tubeAt( move->tubeTo() )->extractColor());
        }
        removeLastMove();
        emit movesChanged();
        emit needToRefresh();
    }
}

void Game::startAgain()
{
    while (hasMoves()) {
        MoveItem * move = moves()->current();
        if (move) {
            for (int i = 0; i < move->count(); ++i) {
                m_board->tubeAt( move->tubeFrom() )->putColor(
                            m_board->tubeAt( move->tubeTo() )->extractColor());
            }
            removeLastMove();
        }
    }

    emit movesChanged();
    emit needToRefresh();
}

void Game::onApplicationStateChanged()
{
    if (QGuiApplication::applicationState() != Qt::ApplicationActive)
        if (!m_board->isSolved())
            saveTemporary();
}
