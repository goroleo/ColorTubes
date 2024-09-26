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

    BoardModel(MoveItem * fromMove = nullptr);
    ~BoardModel();

    BoardModel  * rootBoard()   const { return m_rootBoard; }
    BoardModel  * parentBoard() const { return m_parentBoard; }
    MoveItem    * parentMove()  const { return m_parentMove; }

    void          clear();
    void          cloneFrom(const BoardModel &boardToClone);
    bool          isSolved();
    int           level() const { return m_level; }
    void          setLevel(int newLevel) { m_level = newLevel; }

    int           tubesCount() const { return m_tubes->size(); }
    TubeModel   * tubeAt(int index) const;
    void          addNewTube();
    void          addNewTube(const TubeModel &tubeToClone);
    void          addNewTube(const quint32 storedTube);

    quint16       calculateMoves();
    bool          hasMoves();
    int           movesCount();
    MoveItem    * currentMove();
    void          removeCurrentMove();
    void          removeChildrenMoves();
    quint32       getMoveData(const TubeModel &tubeFrom, const TubeModel &tubeTo);

    void          randomFill(int fillTubes, int emptyTubes);

    quint32       hash() const { return m_crc32; } // has to be calculated before
    void          calculateHash();

    QString       toString() const;

private:
    BoardModel  * m_parentBoard = nullptr;
    BoardModel  * m_rootBoard = nullptr;
    MoveItem    * m_parentMove = nullptr;

    TubeModels  * m_tubes = nullptr;
    MoveItems   * m_moves = nullptr;
    qint32        m_level;
    quint32       m_crc32 = 0;  // hash value comparing to another board

    MoveItems   * moves();
    MoveItem    * addNewMove(const TubeModel &tubeFrom, const TubeModel &tubeTo);
    bool          canDoMove(const TubeModel &tubeFrom, const TubeModel &tubeTo);
    quint8        colorsToMove(const TubeModel &tubeFrom, const TubeModel &tubeTo);

    bool          checkFilledTubes();
    void          fillActiveColors();
};

QDebug operator << (QDebug dbg, const BoardModel &board);
QDebug operator << (QDebug dbg, const BoardModel *board);

#endif // BOARDMODEL_H
