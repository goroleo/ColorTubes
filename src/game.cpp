#include "game.h"

#include "core/jctlformat.h"
#include "core/boardmodel.h"
#include "core/tubemodel.h"
#include "core/usedcolors.h"
//#include "gui/tubeflyer.h"
#include "gui/shadelayer.h"
#include "gui/bottlelayer.h"
#include "gui/colorslayer.h"
#include "gui/corklayer.h"

Game* Game::m_instance = nullptr;

Game::~Game()
{
    delete usedColors;
    delete jctl;
    delete board;

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
    board = new BoardModel();

    qmlRegisterType<ShadeLayer>("ShadeLayer",1,0,"ShadeLayer");
    qmlRegisterType<BottleLayer>("BottleLayer",1,0,"BottleLayer");
    qmlRegisterType<ColorsLayer>("ColorsLayer",1,0,"ColorsLayer");
    qmlRegisterType<CorkLayer>("CorkLayer",1,0,"CorkLayer");
}

