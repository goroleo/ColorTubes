#ifndef CTPALETTE_H
#define CTPALETTE_H

#include <QColor>

class CtPalette
{
public:
    static CtPalette & create();
    static CtPalette & instance();
    ~CtPalette();

//  int    size();    // use CT_NUM_OF_COLORS instead

    QColor getColor(int index);
    void   setColor(int index, quint32 rgb);

    void   setDefault();
    void   setDefault(bool lightTheme);
    bool   load();
    bool   save();

private:
    CtPalette() {};
    void   initialize();
    quint32 * m_items;

    static CtPalette * m_instance;
};


#endif // CTPALETTE_H
