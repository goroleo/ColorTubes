#include "usedcolors.h"
#include "src/ctglobal.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    #include <QRandomGenerator>
#endif


UsedColors::UsedColors()
{
    m_items = new quint8[CtGlobal::NUM_OF_COLORS];
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        m_items[i] = 0;
    }
}

UsedColors::~UsedColors()
{
    delete[] m_items;
}

int UsedColors::size()
{
    return CtGlobal::NUM_OF_COLORS;
}

void UsedColors::incUsed(quint8 colorNum)
{
    if (colorNum > 0 && colorNum <= CtGlobal::NUM_OF_COLORS)
        m_items[colorNum-1] += 1;
}

void UsedColors::incUsed(quint8 colorNum, quint8 count)
{
    if (colorNum > 0 && colorNum <= CtGlobal::NUM_OF_COLORS)
        m_items[colorNum-1] += count;
}

void UsedColors::decUsed(quint8 colorNum)
{
    if (colorNum > 0 && colorNum <= CtGlobal::NUM_OF_COLORS)
        m_items[colorNum-1] -= 1;
}

void UsedColors::setUsed(quint8 colorNum, quint8 count)
{
    if (colorNum > 0 && colorNum <= CtGlobal::NUM_OF_COLORS)
        m_items[colorNum-1] = count;
}

quint8 UsedColors::getUsed(quint8 colorNum)
{
    if (colorNum > 0 && colorNum <= CtGlobal::NUM_OF_COLORS)
        return m_items[colorNum-1];
    else
        return 0;
}

void UsedColors::clearUsed(quint8 colorNum)
{
    if (colorNum > 0 && colorNum <= CtGlobal::NUM_OF_COLORS)
        m_items[colorNum-1] = 0;
}

void UsedColors::clearAllUsed()
{
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        m_items[i] = 0;
    }
}

quint8 UsedColors::numberOfUsedColors()
{
    int c = 0;
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        if (m_items[i] > 0)
            c++;
    }
    return c;
}

quint8 UsedColors::numberOfFilledColors()
{
    int c = 0;
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        if (m_items[i] == 4)
            c++;
    }
    return c;
}

quint8 UsedColors::numberOfUnusedColors()
{
    int c = 0;
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        if (m_items[i] == 0)
            c++;
    }
    return c;
}

quint8 UsedColors::numberOfUsedCells()
{
    int c = 0;
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        c += m_items[i];
    }
    return c;
}

quint8 UsedColors::numberOfAvailableCells()
{
    int c = 0;
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        c += (4 - m_items[i]);
    }
    return c;
}

quint8 UsedColors::getRandomColor()
{
    int clrIndex;

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QRandomGenerator *rg = QRandomGenerator::global();
    clrIndex = rg->bounded(0, numberOfAvailableCells());
#else
    qsrand(QTime::currentTime().msec());
    clrIndex = qrand() % numberOfAvailableCells();
#endif

    int temp = 0;
    quint8 color = 0;

    do {
        temp += (4 - m_items[color]);
        color++;
    } while (temp < clrIndex + 1);

    return color;
}

