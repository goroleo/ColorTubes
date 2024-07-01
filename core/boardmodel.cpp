﻿#include "boardmodel.h"
#include "src/ctglobal.h"
#include "src/game.h"
#include "core/usedcolors.h"
#include "core/jctlformat.h"

BoardModel::BoardModel()
{
    m_moves = new MoveItems;
    m_tubes = new TubeModels;
}

BoardModel::BoardModel(BoardModel * parentBoard)
{
    BoardModel();
    m_parentBoard = parentBoard;
    m_parentMove = parentBoard->currentMove();
    m_rootBoard = parentBoard->rootBoard();
}

BoardModel::BoardModel(MoveItem * parentMove)
{
    BoardModel();
    m_parentMove = parentMove;
    m_parentBoard = parentMove->boardBefore();
    m_rootBoard = m_parentBoard->rootBoard();
}

BoardModel::~BoardModel()
{
    clear();
    delete m_tubes;
    delete m_moves;
}

void BoardModel::clear()
{
    clearTubes();
    clearMoves();

    m_rootBoard = nullptr;
    m_parentBoard = nullptr;
    m_parentMove = nullptr;
}

void BoardModel::clearTubes()
{
    qDeleteAll(m_tubes->begin(), m_tubes->end());
    m_tubes->clear();
}

void BoardModel::clearMoves()
{
    qDeleteAll(m_moves->begin(), m_moves->end());
    m_moves->clear();
}

MoveItem * BoardModel::currentMove()
{
    return (m_moves->current());
}

void BoardModel::deleteCurrentMove()
{
    if (!m_moves->empty()) {
        delete m_moves->last();
        m_moves->remove(m_moves->size() - 1);
    }
}

bool BoardModel::operator == (const BoardModel & other) const
{
    if (tubesCount() != other.tubesCount())
        return false;

    bool result = true;
    quint32 * stored1 = new quint32[tubesCount()];
    quint32 * stored2 = new quint32[tubesCount()];

    for (int i = 0; i < tubesCount(); ++i) {
        stored1[i] = tubeAt(i)->store();
        stored2[i] = other.tubeAt(i)->store();
    }

    int j1, j2;
    quint32 temp1, temp2;

    // sort stored1 & stored2
    for (int i = 1; i < tubesCount(); ++i) {
        temp1 = stored1[i];
        temp2 = stored2[i];
        j1 = j2 = i;
        while ( (j1 > 0) && (stored1[j1-1] < temp1) ) {
            stored1[j1] = stored1[j1-1];
            j1--;
        }
        while ( (j2 > 0) && (stored2[j2-1] < temp2) ) {
            stored2[j2] = stored2[j2-1];
            j2--;
        }
        stored1[j1] = temp1;
        stored2[j2] = temp2;
    }

    // compare sorted
    j1 = 0;
    while (result && j1 < tubesCount()) {
        if (stored1[j1] != stored2[j1])
            result = false;
        j1++;
    }

    delete [] stored1;
    delete [] stored2;

    return result;
}

TubeModel * BoardModel::tubeAt(int index) const
{
    if (index >= 0 && index < m_tubes->size())
        return m_tubes->at(index);
    return nullptr;
}

TubeModel * BoardModel::addNewTube()
{
    TubeModel * tube = new TubeModel();
    m_tubes->append(tube);
    return tube;
}

TubeModel * BoardModel::addNewTube(TubeModel * tubeToClone)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(tubeToClone);
    m_tubes->append(tube);
    return tube;
}

TubeModel * BoardModel::addNewTube(quint32 storedTube)
{
    TubeModel * tube = new TubeModel();
    tube->assignColors(storedTube);
    m_tubes->append(tube);
    return tube;
}

QString BoardModel::toString() {

    if (m_tubes->isEmpty())
        return QString("");

    QString str("");

    str.append(CtGlobal::endOfLine()).append("  ");
    for (int i = 0; i < m_tubes->size(); i++) {
        str.append(QString::number(i, 16)).append("   ");
    }
    str.append(CtGlobal::endOfLine());

    for (int i = 3; i >= 0; i--) {
        str.append("| ");

        for (quint8 j = 0; j < m_tubes->size(); ++j) {
            if (tubeAt(j)->color(i) != 0) {
                str.append(QString::number(tubeAt(j)->color(i), 16));
            } else {
                str.append(" ");
            }
            str.append(" | ");
        }
        str.append(CtGlobal::endOfLine());
    }
    return str;
}

bool BoardModel::isSolved()
{
    bool result = true;
    quint8 index = 0;

    while (result && index < m_tubes->size()) {
        if (m_tubes->at(index)->state() == CT_STATE_FILLED
                || m_tubes->at(index)->state() == CT_STATE_REGULAR)
            result = false;
        index ++;
    }
    return result;
}

bool BoardModel::canDoMove(int tubeFromIndex, int tubeToIndex)
{
    if ( tubeFromIndex < 0 || tubeFromIndex >= m_tubes->size()
         || tubeToIndex < 0 || tubeToIndex >= m_tubes->size())
        return false;

    return canDoMove(m_tubes->at(tubeFromIndex), m_tubes->at(tubeToIndex));
}

bool BoardModel::canDoMove(TubeModel * tubeFrom, TubeModel * tubeTo)
{
    if (tubeFrom && tubeTo)
        return tubeTo->canPutColor(tubeFrom->currentColor());
    return false;
}

quint8 BoardModel::colorsToMove(int tubeFromIndex, int tubeToIndex)
{
    if ( tubeFromIndex < 0 || tubeFromIndex >= m_tubes->size()
         || tubeToIndex < 0 || tubeToIndex >= m_tubes->size())
        return false;
    return colorsToMove(m_tubes->at(tubeFromIndex), m_tubes->at(tubeToIndex));
}

quint8 BoardModel::colorsToMove(TubeModel * tubeFrom, TubeModel * tubeTo)
{
    if (canDoMove(tubeFrom, tubeTo))
        return qMin(tubeFrom->sameColorCount(),
                    tubeTo->rest());
    else
        return 0;
}

quint32 BoardModel::getMove(int tubeFromIndex, int tubeToIndex)
{
    if (!canDoMove(tubeFromIndex, tubeToIndex))
        return 0;

    MoveItem::MoveData move;
    move.fields.tubeFrom = tubeFromIndex;
    move.fields.tubeTo = tubeToIndex;
    move.fields.count = colorsToMove(tubeFromIndex, tubeToIndex);
    move.fields.color = m_tubes->at(tubeFromIndex)->currentColor();
    return move.stored;
}

quint32 BoardModel::addNewMove(int tubeFromIndex, int tubeToIndex)
{
    MoveItem * move = new MoveItem(this, tubeFromIndex, tubeToIndex);
    m_moves->append(move);
    return move->stored();
}

quint32 BoardModel::undoMove()
{
    MoveItem * move = currentMove();
    if (move) {
        quint32 result = move->stored();
        for (int i = 0; i < move->count(); ++i) {
            m_tubes->at(move->tubeTo())->extractColor();
            m_tubes->at(move->tubeFrom())->putColor(move->color());
        }
        deleteCurrentMove();
        return result;
    }
    return 0;
}

void BoardModel::startAgain()
{
    MoveItem * move;
    while (currentMove()) {
        move = currentMove();
        for (int i=0; i < move->count(); ++i) {
            m_tubes->at(move->tubeTo())->extractColor();
            m_tubes->at(move->tubeFrom())->putColor(move->color());
        }
        deleteCurrentMove();
    }
}

void BoardModel::randomFill(int fillTubes, int emptyTubes)
{
    do {
        clear();
        CtGlobal::game().usedColors()->clearAllUsed();

        m_rootBoard = this;
        for (int i = 0; i < fillTubes + emptyTubes; ++i)
            addNewTube();

        for (int i = 0; i < fillTubes; ++i) {
            for (int c = m_tubes->at(i)->count(); c < 4; ++c) {
                quint8 clr = CtGlobal::game().usedColors()->getRandomColor();
                m_tubes->at(i)->putColor(clr);
                CtGlobal::game().usedColors()->incUsed(clr);
                if (CtGlobal::game().usedColors()->numberOfUsedColors() >= fillTubes) {
                    CtGlobal::game().usedColors()->disableUnusedColors();
                }
            }
        }
   } while (!checkBoard());
}

bool BoardModel::checkBoard()
{
    if (m_tubes->isEmpty())
        return false;
    int i = 0;
    while (i < m_tubes->size()) {
        if (m_tubes->at(i)->isDone())
            return false;
        i++;
    }
    CtGlobal::game().jctl()->storeGame(this);
    return CtGlobal::game().jctl()->checkTubes();
}
