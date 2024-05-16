#include "ctglobal.h"

#include <QString>
#include <QDebug>

#include "io.h"
#include "palette.h"
#include "tubeimages.h"
#include "game.h"

int game_mode = END_GAME;
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
    m_game.instance().~Game();
    m_images.instance().~TubeImages();
    m_palette.instance().~Palette();
    m_io.instance().~Io();
}

int CtGlobal::gameMode()
{
    return m_game.instance().mode();
}

Game& CtGlobal::game()
{
    return m_game.instance();
}

BoardModel * CtGlobal::board()
{
    return m_game.instance().boardModel();
}

Io& CtGlobal::io()
{
    return m_io.instance();
}

Palette& CtGlobal::palette()
{
    return m_palette.instance();
}

QColor CtGlobal::paletteColor(quint8 colorIndex)
{
    return m_palette.instance().getColor(colorIndex);
}

TubeImages& CtGlobal::images()
{
    return m_images.instance();
}

qreal CtGlobal::tubeWidth()
{
    return m_images.instance().tubeWidth();
}

qreal CtGlobal::tubeHeight()
{
    return m_images.instance().tubeHeight();
}

qreal CtGlobal::scale()
{
    return m_images.instance().scale();
}

QString CtGlobal::localFile(QString fName)
{
    return m_io.instance().localFileName(fName);
}

QString CtGlobal::paletteFile()
{
    return m_io.instance().paletteFileName();
}

QString CtGlobal::settingsFile()
{
    return m_io.instance().settingsFileName();
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
    QString s;
    s.setNum(value, 10);
    return s;
}

QString CtGlobal::intToHex(int value)
{
    QString s;
    s.setNum(value, 16);
    return s;
}

QString CtGlobal::endOfLine()
{
    const QChar newline('\n');
    const QChar cr('\r');

    #ifdef __linux__
        const QString eol = QString(newline);
    #elif _WIN32
        const QString eol = QString(cr) + QString(newline);
    #else
        const QString eol = QString(cr);
    #endif
    return eol;
}
