#ifndef USEDCOLORS_H
#define USEDCOLORS_H

#include <QtCore>

class UsedColors
{
public:
    UsedColors();
    ~UsedColors();
//  int     size();                                 // use CT_NUM_OF_COLORS instead

    void    incUsed(quint8 colorNum);
    void    incUsed(quint8 colorNum, quint8 count);
    void    decUsed(quint8 colorNum);               // reserved for FILL_MODE if I'll realize it...
    void    setUsed(quint8 colorNum, quint8 count); // reserved for FILL_MODE also
    quint8  getUsed(quint8 colorNum);

//  void    clearUsed(quint8 colorNum);             // unused?
    void    clear();

//  quint8  numberOfFilledColors();                 // unused?
    quint8  numberOfUsedColors();
//  quint8  numberOfUnusedColors();                 // unused?
//  quint8  numberOfUsedCells();                    // unused?
    quint8  numberOfAvailableCells();

    void    disableUnusedColors();
    quint8  getRandomColor();

private:
    quint8 * m_items;
};

#endif // USEDCOLORS_H
