#include "jctlformat.h"
#include "usedcolors.h"
#include <QDataStream>

#include "src/ctglobal.h"
#include "src/game.h"
#include "boardmodel.h"
#include "tubemodel.h"
#include "moveitem.h"

JctlFormat::JctlFormat()
{
    formatVersion = 2;
    gameMode = CT_PLAY_MODE;
    storedTubes = new QVector<quint32>{};
    storedMoves = new QVector<quint32>{};
}

JctlFormat::~JctlFormat()
{
    clear();
    delete storedTubes;
    delete storedMoves;
}

void JctlFormat::clear()
{
    level = 0;
    gameMode = 0;
    tubesCount = 0;
    emptyCount = 0;
    movesCount = 0;
    movesDone = 0;
    storedTubes->clear();
    storedMoves->clear();
    crc = 0;
}

quint32 JctlFormat::size()
{
    return size(formatVersion);
}


quint32 JctlFormat::size(quint32 version)
{
    switch (version) {
    case 1:
        return (6 + tubesCount) * 4;
    case 2:
        return (8 + tubesCount + movesCount) * 4 + 2;
    default:
        return 0;
    }
}

quint32 JctlFormat::crcVersion1()
{
    quint32 result;
    quint32 dw; // double word
    result = 0x6a + 0x63 + 0x74 + 0x6c + 0x1a
        + formatVersion + fileSize + tubesCount;
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
            if (doXOR)                   //   if doXOR than
                crc ^= 0xa001;           //     crc = crc XOR polynomial 0xA001
        }
        pos++;                           // next buffer position
    }
    return crc;
}

bool JctlFormat::writeTo(QByteArray &buffer)
{
    return writeTo(buffer, 2);
}

bool JctlFormat::writeTo(QByteArray &buffer, quint32 version)
{

    if (version != 1 && version != 2) {
        return false;
    }

    QDataStream data(&buffer, QIODevice::WriteOnly);

    formatVersion = version;
    fileSize = size();
    quint16 zeroWord = 0;                   // 16 bit unsigned zero

    data << JCTL_FILE_ID;
    data << JCTL_FILE_EOF;
    data << formatVersion;
    data << fileSize;

    if (formatVersion == 2) {
        data << level;
        data << gameMode;
    }

    if (formatVersion == 2 && gameMode == CT_FILL_MODE) {
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
        crc = crcVersion1();
        data << (quint16) (crc & 0xffff);
    } else { // if (ver == 2)
        crc = crcVersion2(buffer);
        data << (quint16) (crc & 0xffff);
    }

    return true;
}

bool JctlFormat::readFrom(QByteArray &buffer)
{
    bool result = true;

    quint32 dWord;                  // double word unsigned 32 bits
    quint16 word;                   // word unsigned 16 bits

    QDataStream data(&buffer, QIODevice::ReadOnly);

    data >> dWord;
    result = (dWord == JCTL_FILE_ID);

    if (result) {
        data >> dWord;
        result = (dWord == JCTL_FILE_EOF);
    }

    if (result) {
        data >> formatVersion;
        result = (formatVersion == 1 || formatVersion == 2);
    }

    if (result) {
        data >> fileSize;
        result = (fileSize = buffer.size());
    }

    if (result) {

        if (formatVersion > 1) {
            data >> level;
            data >> gameMode;
            if (gameMode == 0) {
                gameMode = CT_PLAY_MODE;
            }
        } else {
            level = 0;
            gameMode = CT_PLAY_MODE;
        }

        if (gameMode == CT_FILL_MODE) {
            data >> emptyCount;       // read empty tubes count
        } else {
            data >> word;             // empty tubes count is null
            emptyCount = 0;
        }
        data >> tubesCount;           // read all tubes count

        if (formatVersion > 1) {
            data >> movesDone;
            data >> movesCount;

            if (movesDone == 0 && gameMode != CT_ASSIST_MODE) {
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
            if (gameMode != CT_FILL_MODE) {
                result = checkTubes();
            }
        }
    }

    if (result && formatVersion > 1) {
        storedMoves->clear();
        if (movesCount > 0)
        {
            for (int i = 0; i < movesCount; i++) {
                data >> dWord;
                storedMoves->append(dWord);
                result = result
                         && ((dWord >> 24) & 0xff) < tubesCount   // check tubeFrom
                         && ((dWord >> 16) & 0xff) < tubesCount   // check tubeTo
                         && ((dWord >> 8) & 0xff) > 0             // count
                         && (dWord & 0xff) > 0;                   // color number
            }
        }
    }

    if (result) {
        if (formatVersion == 1) {
            data >> crc;                                      // CRC ver 1
            result = (crc == crcVersion1());
        } else if (formatVersion == 2) {
            data >> word;
            crc = word & 0xffff;                              // CRC ver 2
            result = (word == crcVersion2(buffer, fileSize - 2));
        }
    }

    if (!result) {
        clear();
    }

    return result;
}

bool JctlFormat::checkTubes()
{
    if (tubesCount == 0 || tubesCount != storedTubes->size())
        return false;

    quint32 stored; // one of stored tubes
    quint8 color;

    CtGlobal::game().usedColors()->clearAllUsed();

    // fill used colors array
    for (quint16 i = 0; i < tubesCount; i++) {
        stored = storedTubes->at(i);
        while (stored > 0) {
            color = stored & 0xff;
            if (color > 0)
                CtGlobal::game().usedColors()->incUsed(color);
            stored >>= 8;
        }
    }

    // check used colors array
    color = 0;
    do {
        color ++;
        if (CtGlobal::game().usedColors()->getUsed(color) != 0
            && CtGlobal::game().usedColors()->getUsed(color) != 4)
        {
            return false;
        }
    } while (color < CtGlobal::game().usedColors()->size());

    return true;
}

void JctlFormat::storeGame() {
    storeGame(CtGlobal::board());
}

void JctlFormat::storeGame(BoardModel * model) {

    level = 0;
    gameMode = 0;

    tubesCount = model->tubesCount();
    emptyCount = 0;
    storedTubes->clear();
    for (quint16 i = 0; i < model->tubesCount(); i++) {
        storedTubes->append(model->tubeAt(i)->store());
    }

    movesCount = 0;
    movesDone = 0;
    storedMoves->clear();
}

void JctlFormat::storeMoves()
{
    storeGame(CtGlobal::board());
}

void JctlFormat::storeMoves(GameMoves * moves)
{

}

void JctlFormat::restoreGame()
{
    restoreGame(CtGlobal::board());
}

void JctlFormat::restoreGame(BoardModel * boardModel)
{
    boardModel->clear();
    for (quint16 i = 0; i < tubesCount; i++) {
        boardModel->addNewTube(storedTubes->at(i));
    }
}

void JctlFormat::restoreMoves()
{

}

void JctlFormat::restoreMoves(GameMoves * moves)
{

}

