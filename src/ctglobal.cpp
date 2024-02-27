#include "ctglobal.h"

#include <QString>
#include <QObject>

#include "io.h"
#include "palette.h"
#include "tubeimages.h"
#include "game.h"

int game_mode = CtGlobal::END_GAME;
bool created = false;

extern Game m_game;
extern Io m_io;
extern TubeImages m_images;
extern Palette m_palette;

void CtGlobal::create()
{
    m_io.create();      // IO must be first

    m_palette.create(); // Palette must be after IO and before Game
    m_images.create();  // TubeImages must be after IO and before Game

    m_game.create();
    created = true;
}

void CtGlobal::destroy()
{
    created = false;
}

int CtGlobal::gameMode()
{
    return game().mode();
}

Game& CtGlobal::game()
{
    return m_game.instance();
}

Io& CtGlobal::io()
{
    return m_io.instance();
}

Palette& CtGlobal::palette()
{
    return m_palette.instance();
}

TubeImages& CtGlobal::images()
{
    return m_images.instance();
}

QString CtGlobal::localFile(QString fName)
{
    if (m_io.instance().created())
        return m_io.instance().localFileName(fName);
    else
        return QString("");
}

QString CtGlobal::paletteFile()
{
    if (m_io.instance().created())
        return m_io.instance().paletteFileName();
    else
        return QString("");
}

QString CtGlobal::settingsFile()
{
    if (m_io.instance().created())
        return m_io.instance().settingsFileName();
    else
        return QString("");
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
    QString s;
    s.setNum(value & 0xffffff, 16);
    while (s.length() < 6)
        s = "0" + s;
    s = "#" + s;
    return s;
}

QString CtGlobal::intToStr(int value)
{
    QString s;
    s.setNum(value, 10);
    return s;
}
