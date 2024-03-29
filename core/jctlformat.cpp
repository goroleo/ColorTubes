#include "jctlformat.h"
#include "usedcolors.h"
#include <QFile>
#include <QDataStream>

#include "src/ctglobal.h"
#include "src/game.h"
#include "boardmodel.h"
#include "tubemodel.h"

JctlFormat::JctlFormat()
{
    fileVersion = 2;
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
    return size(fileVersion);
}


quint32 JctlFormat::size(quint32 formatVersion)
{
    switch (formatVersion) {
    case 1:
        return (6 + tubesCount) * 4;
    case 2:
        return (8 + tubesCount + movesCount) * 4 + 2;
    default:
        return -1;
    }
}

quint32 JctlFormat::crcVersion1()
{
    quint32 result;
    quint32 dw; // double word
    result = 0x6a + 0x63 + 0x74 + 0x6c + 0x1a
        + fileVersion + fileSize + tubesCount;
    for (int i = 0; i < tubesCount; i++) {
        dw = storedTubes->at(i);
        for (int j = 0; j < 4; j++) {
            result += (dw & 0xff) * j * i;
            dw >>= 8;
        }
    }
    return result;
}

quint16 JctlFormat::crcVersion2(QByteArray &buffer)
{
    return crcVersion2(buffer, buffer.size());
}

quint16 JctlFormat::crcVersion2(QByteArray &buffer, quint32 length)
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

bool JctlFormat::write(QByteArray &buffer, quint32 formatVersion)
{

    if (formatVersion != 1 && formatVersion != 2) {
        return false;
    }

    QDataStream data(&buffer, QIODevice::WriteOnly);

    fileVersion = formatVersion;
    fileSize = size();
    quint16 zeroWord = 0;                   // 16 bit unsigned zero

    data << FILE_ID;
    data << FILE_EOF;
    data << formatVersion;
    data << fileSize;

    if (formatVersion == 2) {
        data << level;
        data << gameMode;
    }

    if (formatVersion == 2 && gameMode == 200) {
        data << emptyCount;
    } else {
        data << zeroWord;
    }

    data << tubesCount;

    if (formatVersion == 2) {
        if (movesDone != movesCount) {
            data << movesDone;
        } else {
            data << zeroWord;
        }
        data << movesCount;
    }

    for (int i = 0; i < tubesCount; i++) {
        data << storedTubes->at(i);
    }

    if (formatVersion == 2) {
        for (int i = 0; i < movesCount; i++) {
            data << storedMoves->at(i);
        }
    }

    if (formatVersion == 1) {
        fileCRC = crcVersion1();
        data << (quint16) (fileCRC & 0xffff);
    } else { // if (ver == 2)
        fileCRC = crcVersion2(buffer);
        data << (quint16) (fileCRC & 0xffff);
    }

    return true;
}

bool JctlFormat::read(QByteArray &buffer)
{
    bool result = true;

    quint32 dWord;                  // double word unsigned 32 bits
    quint16 word;                   // word unsigned 16 bits

    QDataStream data(&buffer, QIODevice::ReadOnly);

    data >> dWord;
    result = (dWord == FILE_ID);

    if (result) {
        data >> dWord;
        result = (dWord == FILE_EOF);
    }

    if (result) {
        data >> fileVersion;
        result = (fileVersion == 1 || fileVersion == 2);
    }

    if (result) {
        data >> fileSize;
        result = (fileSize = buffer.size());
    }

    if (result) {

        if (fileVersion > 1) {
            data >> level;            // read level
            data >> gameMode;         // read gameMode
            if (gameMode == 0) {
                gameMode = CtGlobal::PLAY_MODE;
            }
        } else {
            level = 0;
            gameMode = CtGlobal::PLAY_MODE;
        }

        if (gameMode == CtGlobal::FILL_MODE) {
            data >> emptyCount;       // read empty tubes count
        } else {
            data >> word;             // empty tubes count is null
            emptyCount = 0;
        }
        data >> tubesCount;           // read all tubes count

        if (fileVersion > 1) {
            data >> movesDone;
            data >> movesCount;

            if (movesDone == 0 && gameMode != CtGlobal::ASSIST_MODE) {
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
                data >> dWord;
                storedTubes->append(dWord);
            }

            // check tubes for used colors
            // In FILL_MODE not all tubes can be filled completely
            if (gameMode != CtGlobal::FILL_MODE) {
                result = checkTubes();
            }
        }
    }

    if (result && fileVersion > 1) {
        storedMoves->clear();
        if (movesCount > 0)
        {
            for (int i = 0; i < movesCount; i++) {
                data >> dWord;
                storedMoves->append(dWord);
                result = result
                         && ((dWord >> 24) & 0xff) < tubesCount   // check tubeFrom
                         && ((dWord >> 16) & 0xff) < tubesCount   // check tubeTo
                         && ((dWord >> 8) & 0xff) > 0;            // checks count
            }
        }
    }

    if (result) {
        if (fileVersion == 1) {
            data >> fileCRC;                                      // read CRC ver 1
            result = (fileCRC == crcVersion1());
        } else if (fileVersion == 2) {
            data >> word;
            fileCRC = word & 0xffff;                              // read CRC ver 2
            result = (word == crcVersion2(buffer, fileSize - 2));
        }
    }

    return result;
}

bool JctlFormat::checkTubes()
{
    if (tubesCount == 0 || tubesCount != storedTubes->size())
        return false;

    quint32 stored;                                               // one of stored tubes
    quint8 color;

    CtGlobal::game().usedColors()->clearAllUsed();

    // fill used colors array
    for (quint16 i = 0; i < tubesCount; i++) {
        stored = storedTubes->at(i);
        while (stored > 0)
        {
            color = stored & 0xff;
            if (color > 0)
                CtGlobal::game().usedColors()->incUsed(color);
            stored >>= 8;
        }
    }

    // check used colors array
    color = 0;
    do {
        if (CtGlobal::game().usedColors()->getUsed(color) != 0
            && CtGlobal::game().usedColors()->getUsed(color) != 4)
        {
            return false;
        }
        color ++;
    } while (color < CtGlobal::game().usedColors()->size());

    return true;
}

void JctlFormat::storeGame() {

    level = 0;
    gameMode = 0;

    tubesCount = CtGlobal::game().boardModel()->tubesCount();
    emptyCount = 0;
    storedTubes->clear();
    for (quint16 i = 0; i < CtGlobal::game().boardModel()->tubesCount(); i++) {
        storedTubes->append(CtGlobal::game().boardModel()->getTube(i)->store());
    }

    movesCount = 0;
    movesDone = 0;
    storedMoves->clear();
}
