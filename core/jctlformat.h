#ifndef JCTLFORMAT_H
#define JCTLFORMAT_H

#include <QtCore>

class JctlFormat
{
public:
    JctlFormat();
    ~JctlFormat();

    quint32 fileVersion;
    quint32 fileSize;

    quint32 level = 508;
    quint32 gameMode = 0;
    quint16 tubesCount = 0;
    quint16 emptyCount = 0;
    quint16 movesCount = 0;
    quint16 movesDone = 0;
    QVector<quint32> * storedTubes;
    QVector<quint32> * storedMoves;
    quint32 fileCRC;

    quint32 size();
    quint32 size(quint32 formatVersion);
    quint32 crcVersion1();
    quint16 crcVersion2(QByteArray &buffer);
    quint16 crcVersion2(QByteArray &buffer, quint32 length);

    void storeGame();

    bool read(QByteArray &buffer);
    bool write(QByteArray &buffer);
    bool write(QByteArray &buffer, quint32 formatVersion);

    bool checkTubes();

private:
    static const quint32 FILE_ID = 0x6a63746c;
    static const quint32 FILE_EOF = 0x1a;
    bool loaded;
};

#endif // JCTLFORMAT_H
