#ifndef CTIO_H
#define CTIO_H

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

    QString localDir() {return m_dir;}
    QString localFileName(QString fName) {return m_dir + m_sep + fName;}

    QString paletteFileName()  {return localFileName("palette.json");}
    QString settingsFileName() {return localFileName("settings.json");}
    QString statsFileName()    {return localFileName("statsistics.json");}
    QString anglesFileName()   {return localFileName("tiltangles.json");}
    QString tempFileName()     {return localFileName("tempgame.jctl");}

    bool loadJson(QString fName, QJsonObject &jsonObj);
    bool saveJson(QString fName, QJsonObject &jsonObj);

    bool loadGame(QString fName, QByteArray &buffer);
    bool saveGame(QString fName, QByteArray &buffer);

private:
    CtIo() {};
    void initialize();

    static CtIo * m_instance;
    QString m_dir;
    QString m_sep;
};

#endif // CTIO_H
