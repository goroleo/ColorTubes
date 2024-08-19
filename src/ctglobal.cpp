#include "ctglobal.h"

#include <QString>
#include <QDebug>

#include "ctio.h"
#include "ctpalette.h"
#include "ctimages.h"
#include "game.h"

extern Game m_game;
extern CtIo m_io;
extern CtImages m_images;
extern CtPalette m_palette;

void CtGlobal::create()
{
    m_io.create();      // IO must be first

    m_palette.create(); // Palette must be after IO and before Game
    m_images.create();  // TubeImages must be after IO and before Game

    m_game.create();
}

void CtGlobal::destroy()
{
    m_game.instance().~Game();
    m_images.instance().~CtImages();
    m_palette.instance().~CtPalette();
    m_io.instance().~CtIo();
}

Game& CtGlobal::game()
{
    return m_game.instance();
}

CtIo& CtGlobal::io()
{
    return m_io.instance();
}

CtImages& CtGlobal::images()
{
    return m_images.instance();
}

CtPalette& CtGlobal::palette()
{
    return m_palette.instance();
}

BoardModel * CtGlobal::board()
{
    return m_game.instance().boardModel();
}

MoveItems * CtGlobal::moves()
{
    return m_game.instance().moves();
}

QString CtGlobal::paletteFileName()
{
    return m_io.instance().paletteFileName();
}

QString CtGlobal::settingsFileName()
{
    return m_io.instance().settingsFileName();
}

QString CtGlobal::tempFileName()
{
    return m_io.instance().tempFileName();
}

QColor CtGlobal::paletteColor(quint8 colorIndex)
{
    return m_palette.instance().getColor(colorIndex);
}

quint32 CtGlobal::colorStrToRgb(bool &ok, QString value)
{
    quint32 result = 0;
    int size = value.length();

    if (value.startsWith("#"))
        size --;
    else if (value.startsWith("0x"))
        size -= 2;

    result = value.rightRef(size).toInt(&ok, 16);
    if (!ok)
        result = 0;
    else if (size <= 6)
        result += 0xff000000;
    return result;
}

QString CtGlobal::colorRgbToStr(quint32 value)
{
    QString s = intToHex(value & 0xffffff);
    while (s.length() < 6)
        s.prepend("0");
    s.prepend("#");
    return s;
}

QString CtGlobal::intToStr(int value)
{
    return QString().setNum(value, 10);
}

QString CtGlobal::intToHex(int value)
{
    return QString().setNum(value, 16);
}
