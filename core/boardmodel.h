#ifndef BOARDMODEL_H
#define BOARDMODEL_H

#include <QVector>
#include "tubemodel.h"
#include "moveitem.h"

class MoveItem;

class BoardModel
{
public:
    BoardModel();
    BoardModel(BoardModel * parentBoard);
    BoardModel(MoveItem * parentMove);
    ~BoardModel();

    const BoardModel * rootBoard()   const {return m_rootBoard;}
    const BoardModel * parentBoard() const {return m_parentBoard;}
    const MoveItem   * parentMove()  const {return m_parentMove;}
    MoveItem         * currentMove() const;

    int            tubesCount() const { return m_tubes->size(); }
    TubeModel    * getTube(int index) const;

    int           movesCount() const { return m_moves->size(); }
    bool          hasMoves() const;
    void          deleteCurrentMove();

    int           calculateMoves();

    TubeModel *   addNewTube();
    TubeModel *   addNewTube(TubeModel * tube);
    TubeModel *   addNewTube(quint32 storedValue);

    void          clear();
    void          clearMoves();
    bool          isSolved();

    bool operator == (const BoardModel &other) const;

    QString toString();

//    void sortMoves() {
//        std::sort(moves->begin(), moves->end(), MoveItem);
//    }

private:
    const BoardModel  * m_parentBoard = nullptr;
    const BoardModel  * m_rootBoard = nullptr;
    const MoveItem    * m_parentMove = nullptr;

    QVector<TubeModel> * m_tubes;
    QVector<MoveItem>  * m_moves;


};

#endif // BOARDMODEL_H
