#include "game.h"

#include "src/ctglobal.h"
#include "src/io.h"

#include "core/jctlformat.h"
#include "core/boardmodel.h"
#include "core/tubemodel.h"
#include "core/usedcolors.h"

#include "gui/gameboard.h"
#include "gui/tubeitem.h"

Game * Game::m_instance = nullptr;

Game::~Game()
{
    delete m_usedColors;
    delete m_jctl;
    delete m_board;

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
    if (m_instance == nullptr) {
        m_instance = new Game();
        m_instance->initialize();
    }
    return *m_instance;
}

void Game::initialize()
{
    m_usedColors = new UsedColors; // UsedColors must be before JctlFormat
    m_jctl = new JctlFormat();

    m_board = new BoardModel();
    qmlRegisterType <GameBoard> ("GameBoard", 1, 0, "GameBoard");

    load(QLatin1String(":/jctl/example2.jctl"));
}

void Game::load(QString fileName)
{
    QByteArray buffer;
    bool result;

    result = CtGlobal::io().loadGame(fileName, buffer);

    if (result)
        result = m_jctl->read(buffer);

    if (result) {

        m_board->clear();

        quint32 stored;
        for (quint16 i = 0; i < m_jctl->tubesCount; i++) {
            stored = m_jctl->storedTubes->at(i);
            m_board->addNewTube(stored);
        }

        // TODO: restore moves

    }

    if (result) {
        // signal to redraw GameBoard
        emit onGameLoaded();
    } else
        qDebug() << "unsuccessfuly";

}

void Game::save(QString fileName)
{
    QByteArray buffer;

    m_jctl->storeGame();
    m_jctl->write(buffer);
    CtGlobal::io().saveGame(fileName, buffer);
}
