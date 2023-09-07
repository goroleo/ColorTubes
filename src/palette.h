#ifndef PALETTE_H
#define PALETTE_H

#include <QtGlobal>
#include <QColor>

class Palette
{
public:
    static Palette& create();
    static Palette& instance();
    ~Palette();

    int size();

    QColor background() {return m_background;}
    QColor dialog() {return m_dialog;}

    QColor getColor(int index);
    void setColor(int index, quint32 rgb);

    void setDefault();
    bool load();
    bool save();

private:
    Palette() {};
    void initialize();

    QColor   m_background;
    QColor   m_dialog;
    quint32* m_items;

    static Palette* m_instance;
};


#endif // PALETTE_H
