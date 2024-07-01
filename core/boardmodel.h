#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include <QVector>
#include <iostream>
#include "tubemodel.h"
#include "moveitem.h"

class MoveItem;
class TubeModel;

class BoardModel
{
public:
    BoardModel();
    BoardModel(BoardModel * parentBoard);
    BoardModel(MoveItem * parentMove);
    ~BoardModel();

    BoardModel  * rootBoard()   {return m_rootBoard;}
    BoardModel  * parentBoard() {return m_parentBoard;}
    MoveItem    * parentMove()  {return m_parentMove;}
    MoveItem    * currentMove();

    void          clear();
    void          clearTubes();
    void          clearMoves();

    int           tubesCount() const { return m_tubes->size(); }
    TubeModel   * tubeAt(int index) const;

    TubeModel   * addNewTube();
    TubeModel   * addNewTube(TubeModel * tubeToClone);
    TubeModel   * addNewTube(quint32 storedTube);

    bool          isSolved();

    int           movesCount() { return m_moves->size(); }
    bool          canDoMove(int tubeFromIndex, int tubeToIndex);
    quint8        colorsToMove(int tubeFromIndex, int tubeToIndex);
    bool          canDoMove(TubeModel * tubeFrom, TubeModel * tubeTo);
    quint8        colorsToMove(TubeModel * tubeFrom, TubeModel * tubeTo);
    quint32       getMove(int tubeFromIndex, int tubeToIndex);
    quint32       addNewMove(int tubeFromIndex, int tubeToIndex);
    bool          hasMoves() { return !m_moves->empty(); }
    void          deleteCurrentMove();
    quint32       undoMove();
    void          startAgain();
    MoveItems   * moves() {return m_moves;}

    int           calculateMoves();
    QString       toString();

    void          randomFill(int fillTubes, int emptyTubes);

    int           level() {return m_level; }
    void          setLevel(int newLevel) {m_level = newLevel;}

    bool operator == (const BoardModel &other) const;


private:
    BoardModel  * m_parentBoard = nullptr;
    BoardModel  * m_rootBoard = nullptr;
    MoveItem    * m_parentMove = nullptr;

    TubeModels  * m_tubes;
    MoveItems   * m_moves;
    qint32        m_level;

    bool          checkBoard();

};

#endif // BOARDMODEL_H
