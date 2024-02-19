#include "game.h"

#include "core/jctlformat.h"
#include "core/boardmodel.h"
#include "core/tubemodel.h"
#include "core/usedcolors.h"

#include "gui/tubeitem.h"
#include "gui/gameboard.h"

Game* Game::m_instance = nullptr;

Game::~Game()
{
    delete usedColors;
    delete jctl;
//    delete board;

    delete m_instance;
    m_instance = nullptr;
}

Game& Game::create()
{
    if (m_instance == nullptr) {
        m_instance = new Game();
        m_instance->initialize();
    }
    return *m_instance;
}

Game& Game::instance()
{
    return *m_instance;
}

void Game::initialize()
{
    usedColors = new UsedColors; // UsedColors must be before JctlFormat

    jctl = new JctlFormat();
//    board = new GameBoard();

    qmlRegisterType <TubeItem> ("TubeItem", 1, 0, "TubeItem");
    qmlRegisterType <GameBoard> ("GameBoard", 1, 0, "GameBoard");
}

