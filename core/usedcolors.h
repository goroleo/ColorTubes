#ifndef USEDCOLORS_H
#define USEDCOLORS_H

#include <QtCore>

class UsedColors
{
public:
    UsedColors();
    ~UsedColors();

    int size();

    void incUsed(quint8 colorNum);
    void incUsed(quint8 colorNum, quint8 count);
    void decUsed(quint8 colorNum);
    void setUsed(quint8 colorNum, quint8 count);
    quint8 getUsed(quint8 colorNum);
    void clearUsed(quint8 colorNum);
    void clearAllUsed();

    quint8 numberOfFilledColors();
    quint8 numberOfUsedColors();
    quint8 numberOfUnusedColors();
    quint8 numberOfUsedCells();
    quint8 numberOfAvailableCells();
    quint8 getRandomColor();
    void disableUnusedColors();


private:
    quint8* m_items;

};

#endif // USEDCOLORS_H
