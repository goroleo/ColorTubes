#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include <QtCore>

class TubeModel;
class TubeModels;
class MoveItem;
class MoveItems;

class BoardModel
{
public:

    BoardModel(MoveItem * fromMove = nullptr);
    ~BoardModel();

    BoardModel  * rootBoard()   const { return m_rootBoard; }
    BoardModel  * parentBoard() const { return m_parentBoard; }
    MoveItem    * parentMove()  const { return m_parentMove; }

    void          clear();
    bool          isSolved();

    int           level() const { return m_level; }
    void          setLevel(int newLevel) { m_level = newLevel; }

    int           tubesCount() const;
    TubeModel   * tubeAt(int index) const;
    void          addNewTube();
    void          addNewTube(const TubeModel &tubeToClone);
    void          addNewTube(const quint32 storedTube);
    void          addTubesFrom(const BoardModel &boardFrom);   // a.k.a. "clone board"

    quint16       findMoves();           // calculates and sorts all available moves at this board
    bool          hasMoves();
    int           movesCount();
    MoveItem    * currentMove();         // current move is the last one in the sorted moves list
    void          removeCurrentMove();
    void          removeChildrenMoves(); // removes all children moves and all boards resulted of them
    quint32       getMoveData(const TubeModel &tubeFrom, const TubeModel &tubeTo);

    void          randomFill(int fillTubes, int emptyTubes);

    quint32       hash() const { return m_crc32; }    // call calculateHash() before use this
    void          calculateHash();

    QString       toString() const;

private:
    BoardModel  * m_parentBoard = nullptr;
    BoardModel  * m_rootBoard = nullptr;
    MoveItem    * m_parentMove = nullptr;

    TubeModels  * m_tubes = nullptr;
    MoveItems   * m_moves = nullptr;
    qint32        m_level = 0;
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
