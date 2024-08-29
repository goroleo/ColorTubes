#include "ctio.h"
#include <auroraapp.h>

#include <QIODevice>

#include <QFile>
#include <QJsonDocument>

#include <QDebug>

CtIo * CtIo::m_instance = nullptr;

CtIo::~CtIo()
{
    qDebug() << "IO destroyed";
    m_instance = nullptr;
}

CtIo & CtIo::create()
{
    return instance();
}

CtIo & CtIo::instance()
{
    if (m_instance == nullptr) {
        m_instance = new CtIo();
        m_instance->initialize();
        qDebug() << "IO created";
    }
    return * m_instance;
}

void CtIo::initialize()
{
/*
    qDebug() << "app filesDir(false)" << Application::filesDir(false).path();
    qDebug() << "app filesDir(true)" << Application::filesDir(true).path();
    qDebug() << "app cacheDir(false)" << Application::cacheDir(false).path();
    qDebug() << "app cacheDir(true)" << Application::cacheDir(true).path();

    qDebug() << "org filesDir(false)" << Application::organizationFilesDir(false).path();
    qDebug() << "org filesDir(true)" << Application::organizationFilesDir(true).path();
    qDebug() << "org cacheDir(false)" << Application::organizationCacheDir(false).path();
    qDebug() << "org cacheDir(true)" << Application::organizationCacheDir(true).path();

    qDebug() << "static pathTo(\"\")" << Application::pathTo("").path();
    qDebug() << "static org pathTo(\"\")" << Application::organizationPathTo("").path();

*/
    m_dir = Aurora::Application::filesDir(false);

    if (!m_dir.exists()) {
        m_dir.mkpath(m_dir.path());
        qDebug() << "Creating application path" << m_dir.path();
    } else
        qDebug() << "Application path:" << m_dir.path();
}

QString CtIo::localFileName(QString fName)
{
//    qDebug() << "file mame" << fName << "path" << m_dir.filePath(fName);
    return m_dir.filePath(fName);
}

bool CtIo::tempFileExists()
{
    return QFile::exists(tempFileName());
}

void CtIo::tempFileDelete()
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

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

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

