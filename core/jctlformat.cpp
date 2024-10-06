#include "jctlformat.h"

#include <QDataStream>

#include "src/ctglobal.h"
#include "src/ctio.h"
#include "src/game.h"
#include "usedcolors.h"
#include "boardmodel.h"
#include "tubemodel.h"
#include "moveitem.h"

JctlFormat::JctlFormat()
{
    m_formatVersion = 3;
    m_gameMode = CT_PLAY_MODE;
    m_storedTubes = new QVector<quint32>{};
    m_storedMoves = new QVector<quint32>{};
}

JctlFormat::~JctlFormat()
{
    clear();
    delete m_storedTubes;
    delete m_storedMoves;
}

void JctlFormat::clear()
{
    m_level = 0;
    m_formatVersion = 3;
    m_gameMode = 0;
    m_tubesCount = 0;
    m_emptyCount = 0;
    m_movesCount = 0;
    m_movesDone = 0;
    m_storedTubes->clear();
    m_storedMoves->clear();
    m_crc = 0;
}

quint32 JctlFormat::size()
{
    return size(m_formatVersion);
}

quint32 JctlFormat::size(quint32 formatVersion)
{
    switch (formatVersion) {
    case 1:
        return (6 + m_tubesCount) * 4;
    case 2:
        return (8 + m_tubesCount + m_movesCount) * 4 + 2;
    case 3:
        return (9 + m_tubesCount + m_movesCount) * 4;
    default:
        return 0;
    }
}

quint32 JctlFormat::crcVersion1()
{
    quint32 result;
    quint32 dw; // double word
    result = 0x6a + 0x63 + 0x74 + 0x6c + 0x1a
        + m_formatVersion + m_fileSize + m_tubesCount;
    for (int i = 0; i < m_tubesCount; i++) {
        dw = m_storedTubes->at(i);
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

quint32 JctlFormat::crcVersion3(QByteArray &buffer)
{
    return crcVersion3(buffer, buffer.size());
}

quint32 JctlFormat::crcVersion3(QByteArray &buffer, quint32 length)
{
    // CRC32 algorithm

    quint32 crc = 0xffffffff;
    quint32 pos = 0;

    while (pos < length) {
        quint8 byte = (quint8) buffer.at(pos);
        crc = CtGlobal::crc32Table( (crc ^ byte) & 0xFF ) ^ (crc >> 8);
        pos++;
    }
    crc ^= 0xffffffff;
    return crc;
}

bool JctlFormat::write(QByteArray &bufferTo)
{
    return write(bufferTo, 2);
}

bool JctlFormat::write(QByteArray &bufferTo, quint32 formatVersion)
{
    if (! (formatVersion == 1
            || formatVersion == 2
            || formatVersion == 3))
        return false;

    QDataStream data(&bufferTo, QIODevice::WriteOnly);

    m_formatVersion = formatVersion;
    m_fileSize = size();
    quint16 zeroWord = 0;                   // 16 bit unsigned zero

    data << JCTL_FILE_ID;
    data << JCTL_FILE_EOF;
    data << m_formatVersion;
    data << m_fileSize;

    if (m_formatVersion > 1) {
        data << m_level;
        data << m_gameMode;
    }

    if (m_formatVersion > 1 && m_gameMode == CT_FILL_MODE) {
        data << m_emptyCount;
    } else {
        data << zeroWord;
    }
    data << m_tubesCount;

    if (m_formatVersion > 1) {
        if (m_movesDone != m_movesCount) {
            data << m_movesDone;
        } else {
            data << zeroWord;
        }
        data << m_movesCount;
    }

    for (int i = 0; i < m_tubesCount; i++) {
        data << m_storedTubes->at(i);
    }

    if (m_formatVersion > 1) {
        for (int i = 0; i < m_movesCount; i++) {
            data << m_storedMoves->at(i);
        }
    }

    switch (m_formatVersion) {
    case 1:
        m_crc = crcVersion1();
        data << m_crc;
        break;
    case 2:
        m_crc = crcVersion2(bufferTo);
        data << (quint16) (m_crc & 0xffff);
        break;
    case 3:
        m_crc = crcVersion3(bufferTo);
        data << m_crc;
        break;
    default:
        return false;
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

    if (result) {
        data >> dWord;
        result = (dWord == JCTL_FILE_EOF);
    }

    if (result) {
        data >> m_formatVersion;
        result = (m_formatVersion == 1)
                  || (m_formatVersion == 2)
                  || (m_formatVersion == 3);
    }

    if (result) {
        data >> m_fileSize;
        result = (m_fileSize = bufferFrom.size());
    }

    if (result) {

        if (m_formatVersion > 1) {
            data >> m_level;
            data >> m_gameMode;
            if (m_gameMode == 0) {
                m_gameMode = CT_PLAY_MODE;
            }
        } else {
            m_gameMode = CT_PLAY_MODE;
        }

        if (m_gameMode == CT_FILL_MODE) {
            data >> m_emptyCount;       // read empty tubes count
        } else {
            data >> word;               // empty tubes count is null
        }
        data >> m_tubesCount;

        if (m_formatVersion > 1) {
            data >> m_movesDone;
            data >> m_movesCount;

            if (m_movesDone == 0 && m_gameMode != CT_ASSIST_MODE) {
                m_movesDone = m_movesCount;
            }
            if (m_movesDone > m_movesCount) {
                m_movesDone = 0;
            }
        }
        result = m_fileSize == size();
    }

    if (result) {
        if (m_tubesCount > 0) {
            for (int i = 0; i < m_tubesCount; i++) {
                data >> dWord;
                m_storedTubes->append(dWord);
            }

            // check tubes for used colors
            // In FILL_MODE not all tubes can be filled completely
            if (m_gameMode != CT_FILL_MODE) {
                result = checkTubes();
            }
        }
    }

    if (result && m_formatVersion > 1) {
        if (m_movesCount > 0) {
            for (int i = 0; i < m_movesCount; i++) {
                data >> dWord;

                MoveData move;
                move.stored = dWord;

                if (move.fields.tubeFrom < m_tubesCount
                        && move.fields.tubeTo < m_tubesCount
                        && move.fields.count > 0
                        && move.fields.color > 0)
                {
                    m_storedMoves->append(move.stored);
                } else {
                    result = false;
                }
            }
        }
    }

    if (result) {
        if (m_formatVersion == 1) {
            data >> m_crc;                                      // CRC ver 1
            result = (m_crc == crcVersion1());
        } else if (m_formatVersion == 2) {
            data >> word;
            m_crc = word;                                       // CRC ver 2
            result = (m_crc == crcVersion2(bufferFrom, m_fileSize - 2));
        } else if (m_formatVersion == 3) {
            data >> m_crc;                                      // CRC ver 3
            result = (m_crc == crcVersion3(bufferFrom, m_fileSize - 4));
        }
    }

    if (!result)
        clear();

    return result;
}

bool JctlFormat::checkTubes()
{
    if (m_tubesCount == 0 || m_tubesCount != m_storedTubes->size())
        return false;
    CtGlobal::game().usedColors()->clear();

    // fill used colors array
    for (quint16 i = 0; i < m_tubesCount; i++) {
        quint32 stored = m_storedTubes->at(i); // one of stored tubes
        while (stored > 0) {
            if ((stored & 0xff) > 0)
                CtGlobal::game().usedColors()->incUsed(stored & 0xff);
            stored >>= 8;
        }
    }

    // check used colors array
    for (quint8 i = 0; i < CT_NUMBER_OF_COLORS; ++i) {
        quint8 colorUsed = CtGlobal::game().usedColors()->getUsed(i + 1);
        if (colorUsed != 0 && colorUsed != 4)
            return false;
    }
    return true;
}

void JctlFormat::storeGame()
{
    storeGame(CtGlobal::board());
}

void JctlFormat::storeGame(BoardModel * boardModel)
{
    if (!boardModel)
        return;

    m_gameMode = CtGlobal::game().mode();
    m_level = boardModel->level();

    m_tubesCount = boardModel->tubesCount();
    m_emptyCount = 0;
    m_storedTubes->clear();
    for (quint16 i = 0; i < m_tubesCount; i++)
        m_storedTubes->append(boardModel->tubeAt(i)->store());

    m_movesCount = 0;
    m_movesDone = 0;
    m_storedMoves->clear();
}

void JctlFormat::storeMoves()
{
    storeMoves(CtGlobal::moves());
}

void JctlFormat::storeMoves(MoveItems * moves)
{
    if (!moves)
        return;

    m_movesCount = moves->size();
    m_movesDone = 0;
    m_storedMoves->clear();
    for (quint16 i = 0; i < m_movesCount; i++)
        m_storedMoves->append(moves->at(i)->stored());
}

void JctlFormat::restoreGame()
{
    restoreGame(CtGlobal::board());
}

void JctlFormat::restoreGame(BoardModel * boardModel)
{
    boardModel->clear();
    for (quint16 i = 0; i < m_tubesCount; i++)
        boardModel->addNewTube(m_storedTubes->at(i));

    boardModel->setLevel(m_level);
}

void JctlFormat::restoreMoves()
{
    restoreMoves(CtGlobal::moves());
}

void JctlFormat::restoreMoves(MoveItems * moves)
{
    moves->clear();
    for (quint16 i = 0; i < m_movesCount; i++) {
        MoveItem * move = new MoveItem(m_storedMoves->at(i));
        moves->append(move);
    }
}
