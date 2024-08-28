#include "ctio.h"
#include <sailfishapp.h>

#include <QIODevice>
#include <QDir>
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
    QDir qdir;
    m_sep = qdir.separator();

    QString dirName = SailfishApp::pathToSharedDir().toString();
    if (!qdir.exists(dirName)) {
        qdir.mkpath(dirName);
        qDebug() << "Creating application path" << dirName;
    } else
        qDebug() << "Application path:" << dirName;

    qdir.setPath(dirName);
    m_dir = qdir.path();
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

