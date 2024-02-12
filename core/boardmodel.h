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
    ~BoardModel();

    QVector<TubeModel> * tubes;

    QVector<MoveItem>  * moves;
    MoveItem           * currentMove;
    MoveItem           * parentMove;

    void                 setAsRoot();
    BoardModel           root() const {return * m_root;}
    BoardModel           parent() const {return * m_parent;}
    bool hasMoves();

    int tubesCount() const {return m_tubesCount; }
    void addNewTube();
    void addNewTube(TubeModel tube);

    void clear();
    bool solved();

    bool operator == (const BoardModel &other) const;

//    QString toString();

//    void sortMoves() {
//        std::sort(moves->begin(), moves->end(), MoveItem);
//    }

private:
    BoardModel * m_parent = nullptr;
    BoardModel * m_root = nullptr;
    int m_tubesCount;


};

#endif // BOARDMODEL_H
