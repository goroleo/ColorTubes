#include "boardmodel.h"

BoardModel::BoardModel()
{
    moves = new QVector<MoveItem>;
    tubes = new QVector<TubeModel>;
}


BoardModel::~BoardModel()
{
    delete tubes;

    if (!moves->empty())
        do {
//            moves->begin()->clear();

            moves->remove(0);
        } while (!moves->empty());

    delete moves;
}
