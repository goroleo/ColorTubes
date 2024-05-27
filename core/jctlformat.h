#ifndef JCTLFORMAT_H
#define JCTLFORMAT_H

#include <QtCore>

class BoardModel;
class GameMoves;

/**
 * @brief The JctlFormat class.
 * Java ColorTubes by Legoru (Firstly wrote on java)
 */


/*
    JCTL file format:

    ---------- ver.1 ----------         ---------- ver.2 ----------
    FILE_ID             4 bytes         FILE_ID             4 bytes
    EOF                 4 bytes         EOF                 4 bytes
    FormatVersion       4 bytes         FormatVersion       4 bytes
    FileSize            4 bytes         FileSize            4 bytes

    ---                                 Level               4 bytes
    ---                                 GameMode            4 bytes

    ---                                 EmptyTubesCount     2 bytes
    TubesCount          4 bytes         TubesCount          2 bytes

    ---                                 MovesDone           2 bytes
    ---                                 MovesCount          2 bytes

    TUBES    <TubesCount * 4 b>         TUBES    <TubesCount * 4 b>
    ---                                 MOVES    <MovesCount * 4 b>

    CRC ver.1           4 bytes         CRC ver.2           2 bytes
    ----------  end  ----------         ----------  end  ----------

Notes:
    1) TubesCount is the count of all TUBES at the board.

    2) EmptyTubesCount works when the game is in the manual FILL_MODE,
       and not all tubes are filled yet. It shows how much tubes must
       be staying empty after fills the rest.
       In any other case EmptyTubesCount must be zero.

    3) MovesCount is the count of all stored MOVES, done or calculated.

    4) MovesDone works when the game is in the ASSIST_MODE. In this
       case MOVES array is consist with ALL calculated moves to solve
       the game, instead MovesDone is how much moves from this array
       has done already.
       Otherwise MovesDone will be saved as zero, but when reading it
       must be restored equals to MovesCount.

    5) 4 bytes of every TUBE saves with TubeModel::stored()

    6) 4 bytes of every MOVE saves with MoveItem::stored()

*/

class JctlFormat

{
public:
    JctlFormat();
    ~JctlFormat();

    void storeGame();
    void storeGame(BoardModel * model);
    void storeMoves();
    void storeMoves(GameMoves * moves);
    void restoreGame();
    void restoreGame(BoardModel * model);
    void restoreMoves();
    void restoreMoves(GameMoves * moves);

    bool readFrom(QByteArray & buffer);
    bool writeTo(QByteArray & buffer);
    bool writeTo(QByteArray & buffer, quint32 version);

    void clear();

private:
    static const quint32 JCTL_FILE_ID = 0x6a63746c;  // "JCTL"
    static const quint32 JCTL_FILE_EOF = 0x1a;

    quint32 formatVersion;
    quint32 fileSize;

    quint32 level;
    quint32 gameMode;
    quint16 tubesCount;
    quint16 emptyCount;
    quint16 movesCount;
    quint16 movesDone;
    QVector<quint32> * storedTubes;
    QVector<quint32> * storedMoves;
    quint32 crc;

    quint32 size();
    quint32 size(quint32 version);
    quint32 crcVersion1();
    quint16 crcVersion2(QByteArray &buffer);
    quint16 crcVersion2(QByteArray &buffer, quint32 length);

    bool checkTubes();
};

#endif // JCTLFORMAT_H
