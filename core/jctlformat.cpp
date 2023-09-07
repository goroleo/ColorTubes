#include "jctlformat.h"
#include "usedcolors.h"
#include <QFile>
#include <QDataStream>

#include "src/ctglobal.h"
#include "src/game.h"

JctlFormat::JctlFormat()
{
    fileVer = 2;
    storedTubes = new QVector<quint32>{};
    storedMoves = new QVector<quint32>{};
}

JctlFormat::~JctlFormat()
{
    storedTubes->clear();
    storedMoves->clear();
    delete storedTubes;
    delete storedMoves;
}

quint32 JctlFormat::size()
{
    return size(fileVer);
}


quint32 JctlFormat::size(quint32 formatVer)
{
    switch (formatVer) {
    case 1:
        return (6 + tubesCount) * 4;
    case 2:
        return (8 + tubesCount + movesCount) * 4 + 2;
    default:
        return -1;
    }
}

quint32 JctlFormat::crcVer1()
{
    quint32 result;
    quint32 dw; // double word
    result = 0x6a + 0x63 + 0x74 + 0x6c + 0x1a
        + fileVer + fileSize + tubesCount;
    for (int i = 0; i < tubesCount; i++) {
        dw = storedTubes->at(i);
        for (int j = 0; j < 4; j++) {
            result += (dw & 0xff) * j * i;
            dw >>= 8;
        }
    }
    return result;
}

quint16 JctlFormat::crcVer2(QByteArray &buffer)
{
    return crcVer2(buffer, buffer.size());
}

quint16 JctlFormat::crcVer2(QByteArray &buffer, quint32 length)
{
    // ModBus16 CRC algorithm

    quint16 crc = 0xffff;                // assign start value 0xFFFF
    quint32 pos = 0;                     // current buffer's position
    bool doXOR;                          // see below

    while (pos < length) {

        crc ^= (quint8) buffer.at(pos);  // crc = crc XOR buffer[i]; buffer[i] must be unsigned!

        for (int b = 0; b < 8; b++) {    // for every bit...
            doXOR = ((crc & 1) != 0);    //   doXOR = (last bit of the crc) != 0
            crc >>= 1;                   //   crc = crc >> 1
            if (doXOR) {                 //   if doXOR than
                crc ^= 0xa001;           //     crc = crc XOR polynomial 0xA001
            }
        }
        pos++;                           // next buffer position
    }

    // Now you can get LOW and HIGH bytes of the CRC.
    //   Low byte:    crcLo = crc & 0xff;
    //   High byte:   crcHi = (crc >> 8) & 0xff;
    return crc;
}

bool JctlFormat::write(QByteArray &buffer)
{
    return write(buffer, 2);
}

bool JctlFormat::write(QByteArray &buffer, quint32 ver)
{

    QDataStream ds(&buffer, QIODevice::WriteOnly);

    if (ver != 1 && ver != 2) {
        return false;
    }

    fileVer = ver;
    fileSize = size();
    quint16 zero = 0;

    ds << FILE_ID;
    ds << FILE_EOF;
    ds << ver;
    ds << fileSize;

    if (ver == 2) {
        ds << level;
        ds << gMode;
    }

    if (ver == 2 && gMode == 200) {
        ds << emptyCount;
    } else {
        ds << zero;
    }

    ds << tubesCount;

    if (ver == 2) {
        if (movesDone != movesCount) {
            ds << movesDone;
        } else {
            ds << zero;
        }
        ds << movesCount;
    }

    for (int i = 0; i < tubesCount; i++) {
        ds << storedTubes->at(i);
    }

    if (ver == 2) {
        for (int i = 0; i < movesCount; i++) {
            ds << storedMoves->at(i);
        }
    }

    if (ver == 1) {
        fileCRC = crcVer1();
        ds << (quint16) (fileCRC & 0xffff);
    } else { // if (ver == 2)
        fileCRC = crcVer2(buffer);
        ds << (quint16) (fileCRC & 0xffff);
    }

    return true;
}

bool JctlFormat::read(QByteArray &buffer)
{
    bool result = true;

    quint32 dw; // double word unsigned 32 bits
    quint16 w;  // word unsigned 16 bits

    QDataStream ds(&buffer, QIODevice::ReadOnly);

    ds >> dw;
    result = (dw == FILE_ID);

    if (result) {
        ds >> dw;
        result = (dw == FILE_EOF);
    }

    if (result) {
        ds >> fileVer;
        result = (fileVer == 1 || fileVer == 2);
    }

    if (result) {
        ds >> fileSize;
        result = (fileSize = buffer.size());
    }

    if (result) {

        if (fileVer > 1) {
            ds >> level;         // read level
            ds >> gMode;         // read gameMode
            if (gMode == 0) {
                gMode = CtGlobal::PLAY_MODE;
            }
        } else {
            level = 0;
            gMode = CtGlobal::PLAY_MODE;
        }

        if (gMode == CtGlobal::FILL_MODE) {
            ds >> emptyCount;    // read empty tubes count
        } else {
            ds >> w;             // empty tubes count is null
            emptyCount = 0;
        }
        ds >> tubesCount;        // read all tubes count

        if (fileVer > 1) {
            ds >> movesDone;
            ds >> movesCount;

            if (movesDone == 0 && gMode != CtGlobal::ASSIST_MODE) {
                movesDone = movesCount;
            }
            if (movesDone > movesCount) {
                movesDone = 0;
            }
        } else { //
            movesDone = 0;
            movesCount = 0;
            storedMoves->clear();
        }

        result = fileSize == size();
    }

    if (result) {
        storedTubes->clear();
        if (tubesCount > 0)
        {
            for (int i = 0; i < tubesCount; i++) {
                ds >> dw;
                storedTubes->append(dw);
            }

            // check tubes for used colors
            // In FILL_MODE not all tubes can be filled completely
            if (gMode != CtGlobal::FILL_MODE) {
                result = checkTubes();
            }
        }
    }

    if (result && fileVer > 1) {
        storedMoves->clear();
        if (movesCount > 0)
        {
            for (int i = 0; i < movesCount; i++) {
                ds >> dw;
                storedMoves->append(dw);
                result = result
                         && ((dw >> 24) & 0xff) < tubesCount // check tubeFrom
                         && ((dw >> 16) & 0xff) < tubesCount // check tubeTo
                         && ((dw >> 8) & 0xff) > 0; // checks count
            }
        }
    }

    if (result) {
        if (fileVer == 1) {
            ds >> fileCRC;          // read CRC ver 1
            result = (fileCRC == crcVer1());
        } else if (fileVer == 2) {
            ds >> w;
            fileCRC = w & 0xffff;   // read CRC ver 2
            result = (w == crcVer2(buffer, fileSize - 2));
        }
    }

    return result;
}

bool JctlFormat::checkTubes()
{
    if (tubesCount == 0 || tubesCount != storedTubes->size())
        return false;

    quint32 stored; // one of stored tubes

    CtGlobal::game().usedColors->clearAllUsed();

    quint8 color;

    // fill used colors array
    for (quint16 i = 0; i < tubesCount; i++) {
        stored = storedTubes->at(i);
        while (stored > 0)
        {
            color = stored & 0xff;
            if (color > 0)
                CtGlobal::game().usedColors->incUsed(color);
            stored >>= 8;
        }
    }

    // check used colors array
    color = 0;
    do {
        if (CtGlobal::game().usedColors->getUsed(color) != 0
            && CtGlobal::game().usedColors->getUsed(color) != 4)
        {
            return false;
        }
        color ++;
    } while (color < CtGlobal::game().usedColors->size());

    return true;
}
