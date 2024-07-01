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

int CtGlobal::gameMode()
{
    return m_game.instance().mode();
}

BoardModel * CtGlobal::board()
{
    return m_game.instance().boardModel();
}

CtIo& CtGlobal::io()
{
    return m_io.instance();
}

CtPalette& CtGlobal::palette()
{
    return m_palette.instance();
}

QColor CtGlobal::paletteColor(quint8 colorIndex)
{
    return m_palette.instance().getColor(colorIndex);
}

CtImages& CtGlobal::images()
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

QString CtGlobal::localFileName(QString fName)
{
    return m_io.instance().localFileName(fName);
}

QString CtGlobal::paletteFileName()
{
    return m_io.instance().paletteFileName();
}

QString CtGlobal::settingsFileName()
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
    return QString().setNum(value, 10);
}

QString CtGlobal::intToHex(int value)
{
    return QString().setNum(value, 16);
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
