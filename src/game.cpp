#include "game.h"

#include "src/ctglobal.h"
#include "src/io.h"

#include "core/jctlformat.h"
#include "core/boardmodel.h"
#include "core/tubemodel.h"
#include "core/moveitem.h"
#include "core/usedcolors.h"

#include "gui/gameboard.h"
#include "gui/tubeitem.h"

#include "gui/flowerlayer.h"

Game * Game::m_instance = nullptr;

Game::~Game()
{
    delete m_usedColors;
    delete m_jctl;
    delete m_board;
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
    m_jctl = new JctlFormat();
    m_board = new BoardModel();
    m_moves = new GameMoves();

    load(QLatin1String(":/jctl/example4.jctl"));
}

void Game::load(QString fileName)
{
    QByteArray buffer;
    bool result;

    result = CtGlobal::io().loadGame(fileName, buffer);

    if (result)
        result = m_jctl->readFrom(buffer);

    if (result)
        m_jctl->restoreGame(m_board);

    // TODO: restore moves

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
