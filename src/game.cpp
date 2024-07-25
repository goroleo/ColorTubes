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
    delete m_usedColors;
    delete m_jctl;
    delete m_board;

    clearMoves();
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
        m_instance = new Game();
        m_instance->initialize();
        qDebug() << "Game created";
    }
    return *m_instance;
}

void Game::initialize()
{
    m_usedColors = new UsedColors; // UsedColors must be before JctlFormat
    m_jctl = new JctlFormat;
    m_moves = new MoveItems;
    m_board = new BoardModel;

    load(QLatin1String(":/jctl/example1.jctl"));
}

void Game::load(QString fileName)
{
    QByteArray buffer;
    bool result;

    result = CtGlobal::io().loadGame(fileName, buffer);

    if (result)
        result = m_jctl->readFrom(buffer);

    if (result) {
        m_jctl->restoreGame(m_board);
        m_jctl->restoreMoves(m_moves);
    }

    if (result) {
        // signal to redraw GameBoard
        qDebug() << "game loaded" << fileName;
        emit onGameLoaded();
    } else
        qDebug() << "unsuccessfuly";

}

void Game::save(QString fileName)
{
    QByteArray buffer;

    m_jctl->storeGame();
    m_jctl->storeMoves();
    m_jctl->writeTo(buffer);
    CtGlobal::io().saveGame(fileName, buffer);
}

bool Game::hasMoves()
{
    return !m_moves->empty();
}

void Game::clearMoves()
{
    m_moves->clear();
}

MoveItem * Game::lastMove()
{
    return m_moves->current();
}

void Game::deleteLastMove()
{
    if (!m_moves->isEmpty()) {
        delete m_moves->last();
        m_moves->remove(m_moves->size() - 1);
    }
}

MoveItem * Game::addNewMove(int tubeFromIndex, int tubeToIndex)
{
    MoveItem * move = new MoveItem(m_board, tubeFromIndex, tubeToIndex);
    m_moves->append(move);
    return move;
}
