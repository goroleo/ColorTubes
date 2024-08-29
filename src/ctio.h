#ifndef CTIO_H
#define CTIO_H

#include <QDir>
#include <QJsonObject>

/*!
 * \brief The Input-Output Singleton class
 */

class CtIo
{
public:
    static CtIo & create();
    static CtIo & instance();
    ~CtIo();

    QString localFileName(QString fName);

    QString paletteFileName()  {return localFileName("palette.json");}
    QString settingsFileName() {return localFileName("settings.json");}
    QString statsFileName()    {return localFileName("statsistics.json");}
    QString anglesFileName()   {return localFileName("tiltangles.json");}
    QString tempFileName()     {return localFileName("tempgame.jctl");}
    bool tempFileExists();
    void tempFileDelete();

    bool loadJson(QString fName, QJsonObject &jsonObj);
    bool saveJson(QString fName, QJsonObject &jsonObj);

    bool loadGame(QString fName, QByteArray &buffer);
    bool saveGame(QString fName, QByteArray &buffer);

private:
    CtIo() {};
    void initialize();

    static CtIo * m_instance;
    QDir m_dir;
};

#endif // CTIO_H
