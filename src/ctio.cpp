#include "ctio.h"
#ifdef SAILFISH_OS
    #include <sailfishapp.h>
#else
    #include "auroraapp.h"
#endif

#include <QIODevice>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>

CtIo * CtIo::m_instance = nullptr;

CtIo::~CtIo()
{
    qDebug() << "IO destroyed.";
    m_instance = nullptr;
}

CtIo & CtIo::create()
{
    return instance();
}

CtIo & CtIo::instance()
{
    if (m_instance == nullptr) {
        qDebug() << "Creating input-output.";
        m_instance = new CtIo();
        m_instance->initialize();
    }
    return * m_instance;
}

void CtIo::initialize()
{
/*
    // Aurora's predefined pathes
    qDebug() << "app filesDir(false)" << Aurora::Application::filesDir(false).path();
    qDebug() << "app filesDir(true)"  << Aurora::Application::filesDir(true).path();
    qDebug() << "app cacheDir(false)" << Aurora::Application::cacheDir(false).path();
    qDebug() << "app cacheDir(true)"  << Aurora::Application::cacheDir(true).path();

    qDebug() << "org filesDir(false)" << Aurora::Application::organizationFilesDir(false).path();
    qDebug() << "org filesDir(true)"  << Aurora::Application::organizationFilesDir(true).path();
    qDebug() << "org cacheDir(false)" << Aurora::Application::organizationCacheDir(false).path();
    qDebug() << "org cacheDir(true)"  << Aurora::Application::organizationCacheDir(true).path();

    qDebug() << "static pathTo(\"\")" << Aurora::Application::pathTo("").path();
    qDebug() << "static org pathTo(\"\")" << Aurora::Application::organizationPathTo("").path();

*/
    #ifdef SAILFISH_OS
        m_dir = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    #else
       m_dir = Aurora::Application::filesDir(false);
    #endif

    if (!m_dir.exists()) {
        m_dir.mkpath(m_dir.path());
        qDebug() << "Creating application path" << m_dir.path();
    } else
        qDebug() << "Application path:" << m_dir.path();
}

QString CtIo::localFileName(QString fName)
{
    return m_dir.filePath(fName);
}

bool CtIo::tempFileExists()
{
    return QFile::exists(tempFileName());
}

void CtIo::deleteTempFile()
{
    if (tempFileExists())
        QFile::remove(tempFileName());
}

bool CtIo::loadJson(QString fName, QJsonObject &jsonObj)
{
    QFile loadFile(fName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "LoadJson" << "Couldn't open file" << fName;
        return false;
    }

    QByteArray buffer = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(buffer));

    jsonObj = loadDoc.object();
    return true;
}

bool CtIo::saveJson(QString fName, QJsonObject &jsonObj)
{
    QFile saveFile(fName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "saveJson" << "Couldn't open file" << fName;
        return false;
    }

    saveFile.write(QJsonDocument(jsonObj).toJson(QJsonDocument::Indented));
    return true;
}

bool CtIo::loadGame(QString fName, QByteArray &buffer)
{
    QFile loadFile(fName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "loadGame" << "Couldn't open file" << fName;
        return false;
    }

    buffer = loadFile.readAll();
    return true;
}

bool CtIo::saveGame(QString fName, QByteArray &buffer)
{
    QFile saveFile(fName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "saveGame" << "Couldn't open file" << fName;
        return false;
    }

    saveFile.write(buffer);
    return true;
}
