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

bool BoardModel::operator == (const BoardModel & other) const
{
    if (this->tubesCount() != other.tubesCount())
        return false;

    bool result = true;
    quint32 * stored1 = new quint32[this->tubesCount()];
    quint32 * stored2 = new quint32[this->tubesCount()];

    for (int i = 0; i < tubesCount(); ++i) {
        stored1[i] = tubes->at(i).storeColors();
        stored2[i] = other.tubes->at(i).storeColors();
    }

    std::sort(stored1[0], stored1[this->tubesCount() - 1]);
    std::sort(stored2[0], stored2[this->tubesCount() - 1]);

    for (int i = 0; i < tubesCount(); ++i) {
        if (stored1[i] != stored2[i])
            result = false;
    }

    delete [] stored1;
    delete [] stored2;

    return result;
}
