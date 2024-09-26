#include "jctlformat.h"

#include <QDataStream>

#include "src/ctglobal.h"
#include "src/game.h"
#include "usedcolors.h"
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
    formatVersion = 2;
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

bool JctlFormat::write(QByteArray &bufferTo)
{
    return write(bufferTo, 2);
}

bool JctlFormat::write(QByteArray &bufferTo, quint32 version)
{

    if (version != 1 && version != 2)
        return false;

    QDataStream data(&bufferTo, QIODevice::WriteOnly);

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
        data << crc;
    } else { // if (ver == 2)
        crc = crcVersion2(bufferTo);
        data << (quint16) (crc & 0xffff);
    }

    return true;
}

bool JctlFormat::read(QByteArray &bufferFrom)
{
    bool result = true;

    quint32 dWord;                  // double word unsigned 4 bytes
    quint16 word;                   // word unsigned 2 bytes

    QDataStream data(&bufferFrom, QIODevice::ReadOnly);

    clear();

    data >> dWord;
    result = (dWord == JCTL_FILE_ID);
//    if (result)
//        qDebug() << "JCTL ID is ok.";
//    else
//        qDebug() << "JCTL ID has errors!";

    if (result) {
        data >> dWord;
        result = (dWord == JCTL_FILE_EOF);
//        if (result)
//            qDebug() << "File EOF is ok.";
//        else
//            qDebug() << "File EOF has errors!";
    }

    if (result) {
        data >> formatVersion;
        result = (formatVersion == 1 || formatVersion == 2);

//        if (result)
//            qDebug() << "Format version is ok:" << formatVersion;
//        else
//            qDebug() << "Format version has errors!";
    }

    if (result) {
        data >> fileSize;
        result = (fileSize = bufferFrom.size());

//        if (result)
//            qDebug() << "File size is ok:" << fileSize << "b";
//        else
//            qDebug() << "File size version has errors!";
    }

    if (result) {

        if (formatVersion > 1) {
            data >> level;
            data >> gameMode;
            if (gameMode == 0) {
                gameMode = CT_PLAY_MODE;
            }
        } else {
            gameMode = CT_PLAY_MODE;
        }

        if (gameMode == CT_FILL_MODE) {
            data >> emptyCount;       // read empty tubes count
        } else {
            data >> word;             // empty tubes count is null
        }
        data >> tubesCount;

        if (formatVersion > 1) {
            data >> movesDone;
            data >> movesCount;

            if (movesDone == 0 && gameMode != CT_ASSIST_MODE) {
                movesDone = movesCount;
            }
            if (movesDone > movesCount) {
                movesDone = 0;
            }
        }

//        qDebug().nospace() << "Game mode " << gameMode
//                 << ", Tubes count " << tubesCount << "." << emptyCount
//                 << ", Moves count " << movesCount << "." << movesDone;

        result = fileSize == size();

//        if (result)
//            qDebug() << "All datas are ok.";
//        else
//            qDebug() << "Errors!";
    }


    if (result) {
        if (tubesCount > 0)
        {
            for (int i = 0; i < tubesCount; i++) {
                data >> dWord;
                storedTubes->append(dWord);
//                qDebug() << "Tube" << i << QString::number(dWord, 16);
            }

            // check tubes for used colors
            // In FILL_MODE not all tubes can be filled completely
            if (gameMode != CT_FILL_MODE) {
                result = checkTubes();
            }

//            if (result)
//                qDebug() << "Tubes are ok.";
//            else
//                qDebug() << "Error at Tubes!";
        }
    }

    if (result && formatVersion > 1) {
        if (movesCount > 0)
        {
            for (int i = 0; i < movesCount; i++) {
                data >> dWord;

                MoveData move;
                move.stored = dWord;

                if (move.fields.tubeFrom < tubesCount
                        && move.fields.tubeTo < tubesCount
                        && move.fields.count > 0
                        && move.fields.color > 0) {

//                    qDebug() << "Move" << i << QString::number(dWord, 16) << "is ok.";
                    storedMoves->append(dWord);
                } else {
                    result = false;
//                    qDebug() << "Error at Move" << i << QString::number(dWord, 16);
//                    qDebug().nospace() << "TubeFrom " << QString::number(move.fields.tubeFrom, 16)
//                                       << ", TubeTo " << QString::number(move.fields.tubeTo, 16)
//                                       << ", Color " << QString::number(move.fields.color, 16)
//                                       << ", Count " << move.fields.count;
                }
            }
        }
    }

    if (result) {
        if (formatVersion == 1) {
            data >> crc;                                      // CRC ver 1
            result = (crc == crcVersion1());
        } else if (formatVersion == 2) {
            data >> word;
            crc = word;                                       // CRC ver 2
            result = (crc == crcVersion2(bufferFrom, fileSize - 2));
        }

//        if (result)
//            qDebug() << "CRC is ok.";
//        else
//            qDebug() << "Error at CRC!";
    }

    if (!result)
        clear();

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
    } while (color < CT_NUMBER_OF_COLORS);

    return true;
}

void JctlFormat::storeGame() {
    storeGame(CtGlobal::board());
}

void JctlFormat::storeGame(BoardModel * boardModel) {

    if (!boardModel)
        return;

    gameMode = CtGlobal::game().mode();
    level = boardModel->level();

    tubesCount = boardModel->tubesCount();
    emptyCount = 0;
    storedTubes->clear();
    for (quint16 i = 0; i < tubesCount; i++) {
        storedTubes->append(boardModel->tubeAt(i)->store());
    }

    movesCount = 0;
    movesDone = 0;
    storedMoves->clear();
}

void JctlFormat::storeMoves()
{
    storeMoves(CtGlobal::moves());
}

void JctlFormat::storeMoves(MoveItems * moves)
{
    if (!moves)
        return;

    movesCount = moves->size();
    movesDone = 0;
    storedMoves->clear();
    for (quint16 i = 0; i < movesCount; i++) {
        storedMoves->append(moves->at(i)->stored());
    }
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
    boardModel->setLevel(level);
}

void JctlFormat::restoreMoves()
{
    restoreMoves(CtGlobal::moves());
}

void JctlFormat::restoreMoves(MoveItems * moves)
{
    moves->clear();
    for (quint16 i = 0; i < movesCount; i++) {
        MoveItem * move = new MoveItem(storedMoves->at(i));
        moves->append(move);
    }
}

