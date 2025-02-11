#ifndef JCTLFORMAT_H
#define JCTLFORMAT_H

#include <QtCore>

class BoardModel;
class MoveItems;

/**
 * @brief The JctlFormat class.
 * (J)ava (C)olor(T)ubes by (L)egoru - 'cause I started this in java :)
 */

/*
    JCTL file format:

    ---------- ver.1 ----------         ---------- ver.2,3 ---------
    FILE_ID             4 bytes         FILE_ID              4 bytes
    EOF sign            4 bytes         EOF sign             4 bytes
    Format Version      4 bytes         Format Version       4 bytes
    FileSize            4 bytes         File Size            4 bytes

    ---                                 Level Number         4 bytes
    ---                                 GameMode             4 bytes

    ---                                 EmptyTubesCount      2 bytes
    TubesCount          4 bytes         TubesCount           2 bytes

    ---                                 MovesDone            2 bytes
    ---                                 MovesCount           2 bytes

    TUBES    <TubesCount * 4 b>         TUBES     <TubesCount * 4 b>
    ---                                 MOVES     <MovesCount * 4 b>

    CRC ver.1           4 bytes         CRC ver.2 (Modbus)   2 bytes
                                        or CRC ver.3 (CRC32) 4 bytes
    ----------  end  ----------         -----------  end  ----------

Notes:
    1) TubesCount is the count of all TUBES at the board.

    2) EmptyTubesCount works when the game is in the manual FILL_MODE,
       and not all tubes are filled yet. It shows how much tubes must
       be staying empty after fills the rest.
       In any other case EmptyTubesCount must be zero.

       But I decide not to use manual fill mode in mobile version of
       this game.

    3) MovesCount is the count of all stored MOVES, done or calculated.

    4) MovesDone works when the game is in the ASSIST_MODE. In this
       case MOVES array is consist with ALL calculated moves to solve
       the game, and MovesDone is how much moves from this array has
       done already.
       In any other game modes, except ASSIST_MODE, MovesDone has to be
       saved as zero, but when loading it must be restored equals to
       MovesCount.

    5) 4 bytes of every TUBE saves with TubeModel::stored()

    6) 4 bytes of every MOVE saves with MoveItem::stored()

    7) CRC ver.1 was stupid alot, so in ver.2 it was replaced with the
       16-bit Modbus RTU checksum algorithm.

       In ver.3 I replaced Modbus CRC with CRC32 algorithm. Just because
       Solver uses CRC32 to hash GameBoards (16-bit modbus doesn't fit),
       and it would be a shame to use that table only once. :)

    8) JCTL ver.2 and JCTL ver.3 are absolutely the same format, they
       differ only in the CRC field.

*/

class JctlFormat

{
public:
    JctlFormat();
    ~JctlFormat();

    void storeGame();
    void storeGame(quint32 gameMode);
    void storeGame(BoardModel *boardModel);
    void storeGame(BoardModel *boardModel, quint32 gameMode);

    void storeMoves();
    void storeMoves(quint16 movesCount);
    void storeMoves(MoveItems *moves);
    void storeMoves(MoveItems *moves, quint16 movesCount);

    void restoreGame();
    void restoreGame(BoardModel *boardModel);
    void restoreMoves();
    void restoreMoves(MoveItems *moves);

    int  level() {return m_level;}
    int  gameMode() {return m_gameMode;}
    int  movesDone() {return m_movesDone;}

    bool read(QByteArray &bufferFrom);
    bool write(QByteArray &bufferTo);
    bool write(QByteArray &bufferTo, quint32 formatVersion);

    void clear();
    bool checkTubes();

private:
    static const quint32 JCTL_FILE_ID = 0x6a63746c;  // "JCTL"
    static const quint32 JCTL_FILE_EOF = 0x1a;

    quint32 m_formatVersion;
    quint32 m_fileSize;

    quint32 m_level;
    quint32 m_gameMode;
    quint16 m_tubesCount;
    quint16 m_emptyCount;
    quint16 m_movesCount;
    quint16 m_movesDone;
    QVector<quint32> *m_storedTubes;
    QVector<quint32> *m_storedMoves;
    quint32 m_crc;

    quint32 size();
    quint32 size(quint32 formatVersion);
    quint32 crcVersion1();
    quint16 crcVersion2(QByteArray &buffer);
    quint16 crcVersion2(QByteArray &buffer, quint32 length);
    quint32 crcVersion3(QByteArray &buffer);
    quint32 crcVersion3(QByteArray &buffer, quint32 length);
};

#endif // JCTLFORMAT_H
