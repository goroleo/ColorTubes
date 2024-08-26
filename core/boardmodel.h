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
    BoardModel(MoveItem * move);
    ~BoardModel();

    BoardModel  * rootBoard()   {return m_rootBoard;}
    BoardModel  * parentBoard() {return m_parentBoard;}
    MoveItem    * parentMove()  {return m_parentMove;}
    MoveItem    * currentMove();

    void          clear();

    int           tubesCount() const { return m_tubes->size(); }
    TubeModel   * tubeAt(int index) const;

    TubeModel   * addNewTube();
    TubeModel   * addNewTube(TubeModel * tubeToClone);
    TubeModel   * addNewTube(quint32 storedTube);

    bool          isSolved();

    bool          canDoMove(int tubeFromIndex, int tubeToIndex);
    bool          canDoMove(TubeModel * tubeFrom, TubeModel * tubeTo);
    quint8        colorsToMove(int tubeFromIndex, int tubeToIndex);
    quint8        colorsToMove(TubeModel * tubeFrom, TubeModel * tubeTo);

    quint32       getMoveData(int tubeFromIndex, int tubeToIndex);
    MoveItem    * addNewMove(int tubeFromIndex, int tubeToIndex);

    MoveItems   * moves();
    int           movesCount();
    bool          hasMoves();
    void          deleteCurrentMove();


    quint16       calculateMoves();

    void          randomFill(int fillTubes, int emptyTubes);

    int           level() {return m_level; }
    void          setLevel(int newLevel) {m_level = newLevel;}

//    bool operator == (const BoardModel &other) const;
    QString       toString() const;
    quint16       hash() {return m_hash;}

private:
    BoardModel  * m_parentBoard = nullptr;
    BoardModel  * m_rootBoard = nullptr;
    MoveItem    * m_parentMove = nullptr;

    TubeModels  * m_tubes;
    MoveItems   * m_moves = nullptr;
    qint32        m_level;
    quint16       m_hash;

    bool          checkFilledTubes();
    void          fillActiveColors();
    void          calculateHash();

};

QDebug operator << (QDebug dbg, const BoardModel &board);
QDebug operator << (QDebug dbg, const BoardModel *board);

#endif // BOARDMODEL_H
