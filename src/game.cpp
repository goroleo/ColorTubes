#include "game.h"

#include "src/ctglobal.h"
#include "src/ctio.h"

#include "core/jctlformat.h"
#include "core/boardmodel.h"
#include "core/moveitem.h"
#include "core/usedcolors.h"

Game * Game::m_instance = nullptr;

Game::~Game()
{
    if (!m_board->isSolved())
        saveTemporary();

    delete m_usedColors;
    delete m_jctl;
    delete m_board;

    m_moves->clear();
    delete m_moves;

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
        m_instance->initialize();
    }
    return *m_instance;
}

void Game::initialize()
{
    m_usedColors = new UsedColors; // UsedColors must be before JctlFormat
    m_jctl = new JctlFormat;
    m_moves = new MoveItems;
    m_board = new BoardModel;

    bool loaded = false;
    if (CtGlobal::io().tempFileExists())
        loaded = loadTemporary();

    if (!loaded)
        load(QLatin1String(":/jctl/difficult8.jctl"));
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
        emit gameLoaded();
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

    m_jctl->storeGame();
    m_jctl->storeMoves();
    m_jctl->write(buffer);
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
