#include "game.h"
#include <qguiapplication.h>

#include "src/ctglobal.h"
#include "src/ctio.h"
#include "core/boardmodel.h"
#include "core/tubemodel.h"
#include "core/jctlformat.h"
#include "core/moveitem.h"
#include "core/options.h"
#include "core/solver.h"
#include "core/usedcolors.h"

Game * Game::m_instance = nullptr;

Game::~Game()
{
    if (!m_board->isSolved()) {
        saveTemporary();
        m_options->level = m_board->level();
    }

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

    if (!loaded)
        newLevel();
}

void Game::onApplicationStateChanged()
{
    if (QGuiApplication::applicationState() != Qt::ApplicationActive) {
        m_options->save();
        if (!m_board->isSolved())
            saveTemporary();
    }
}

bool Game::load(QString fileName)
{
    m_gameMode = CT_PLAY_MODE;

    QByteArray buffer;
    bool result;

    result = CtGlobal::io().loadGame(fileName, buffer);

    if (result)
        result = m_jctl->read(buffer);

    if (result) {
        m_jctl->restoreGame(m_board);
        m_gameMode = m_jctl->gameMode();
        m_jctl->restoreMoves(m_moves);
        m_movesDone = m_jctl->movesDone();
    }

    if (result) {
        qDebug() << "Game loaded from" << fileName;
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
    m_jctl->storeGame(CT_PLAY_MODE);
    m_jctl->storeMoves(m_movesDone);
    m_jctl->write(buffer, 2);
    return CtGlobal::io().saveGame(fileName, buffer);
}

bool Game::saveTemporary()
{
    return save(CtGlobal::tempFileName());
}

void Game::removeTemporary()
{
    CtGlobal::io().deleteTempFile();
}

bool Game::isAssistMode()
{
    return m_gameMode == CT_ASSIST_MODE;
}

quint32 Game::level()
{
    return m_board->level();
}

void Game::newLevel()
{
    m_gameMode = CT_PLAY_MODE;
    emit modeChanged();

    if (m_board->tubesCount() > 0) {
        m_options->level = m_board->level();
        m_options->save();
    }
    quint32 levelNumber = m_options->level + 1;
    qDebug () << "New level:" << levelNumber;

    int filledTubes;
    int emptyTubes;

    if (levelNumber < 3) {
        filledTubes = 2;
        emptyTubes = 1;
    } else if (levelNumber < 5) {
        filledTubes = 3;
        emptyTubes = 1;
    } else if (levelNumber < 10) {
        filledTubes = 5;
        emptyTubes = 2;
    } else if (levelNumber < 20) {
        filledTubes = 7;
        emptyTubes = 2;
    } else if (levelNumber < 50) {
        filledTubes = 9;
        emptyTubes = 2;
    } else {
        if (levelNumber & 1) {
            filledTubes = 9;
            emptyTubes = 2;
        } else {
            filledTubes = 12;
            emptyTubes = 2;
        }
    }

    m_board->randomFill(filledTubes, emptyTubes);
    m_board->setLevel(levelNumber);
    emit levelChanged();
    m_moves->clear();
    m_movesDone = 0;
    emit movesChanged();
    saveTemporary();

    if (levelNumber == 1) {
        Solver solver;
        solver.start(m_board);
        for (int i = 0; i < solver.moves()->size(); ++i)
            m_moves->append(new MoveItem(solver.moves()->at(i)->stored()));
        solver.clear();
        startAssistMode();
    }
}


MoveItem * Game::currentMove()
{
    if (m_gameMode == CT_PLAY_MODE) {
        return m_moves->current();
    }
    if (m_gameMode == CT_ASSIST_MODE
            && m_movesDone > 0
            && m_movesDone <= m_moves->size()) {
        return m_moves->at(m_movesDone - 1);
    }
    return nullptr;
}

MoveItem * Game::nextMove()
{
    if (m_gameMode == CT_ASSIST_MODE
            && m_movesDone < m_moves->size()) {
        return m_moves->at(m_movesDone);
    }
    return nullptr;
}

bool Game::movesMade()
{
    return m_movesDone > 0;
}

void Game::removeLastMove()
{
    if (m_gameMode == CT_PLAY_MODE)
        m_moves->removeLast();
    m_movesDone --;
}

void Game::addMove(MoveItem * move)
{
    if (!move)
        return;

    if (m_gameMode == CT_ASSIST_MODE) {
        if (m_movesDone < m_moves->size()
                && move->stored() == m_moves->at(m_movesDone)->stored())
            m_movesDone ++;
        else
            endAssistMode();
    }

    if (m_gameMode == CT_PLAY_MODE) {
        m_moves->append(move);
        m_movesDone ++;
    }

    if (m_movesDone == 1) emit movesChanged();
}

void Game::undoMove()
{
    MoveItem * move = currentMove();
    if (move) {
        for (int i = 0; i < move->count(); ++i) {
            m_board->tubeAt( move->tubeFrom() )->putColor(
                        m_board->tubeAt( move->tubeTo() )->extractColor());
        }
        if (m_gameMode == CT_PLAY_MODE)
            removeLastMove();
        else if (m_gameMode == CT_ASSIST_MODE)
            m_movesDone --;

        if (m_movesDone == 0) emit movesChanged();
    }
}

void Game::startAgain()
{
    while (movesMade()) {
        MoveItem * move = currentMove();
        if (move) {
            for (int i = 0; i < move->count(); ++i) {
                m_board->tubeAt( move->tubeFrom() )->putColor(
                            m_board->tubeAt( move->tubeTo() )->extractColor());
            }
            if (m_gameMode == CT_PLAY_MODE)
                removeLastMove();
            else if (m_gameMode == CT_ASSIST_MODE)
                m_movesDone --;
        }
    }
    emit movesChanged();
}

void Game::checkSolved()
{
    if (m_board->isSolved()) {
        qDebug() << "!!! SOLVED !!!";
        removeTemporary();
        emit levelDone();
    }
}

void Game::solve()
{
    saveTemporary();
    if (m_gameMode == CT_ASSIST_MODE)
        endAssistMode();

    Solver solver;
    solver.start(m_board);

    if (solver.result() == CT_SOLVER_SUCCESS) {
        // need to store solved moves!
        for (int i = 0; i < solver.moves()->size(); ++i)
            m_moves->append(new MoveItem(solver.moves()->at(i)->stored()));
        solver.clear();
        emit solverSuccess();
    } else {
        emit solverError();
    }
}

void Game::startAssistMode()
{
    // all calculated moves will be stored before!
    m_gameMode = CT_ASSIST_MODE;
    emit modeChanged();
}

void Game::endAssistMode()
{
    m_gameMode = CT_PLAY_MODE;
    emit modeChanged();
    while (m_moves->size() > m_movesDone)
        m_moves->removeLast();
    emit movesChanged();
}
