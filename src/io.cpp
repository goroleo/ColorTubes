#include "io.h"
#include "sailfishapp.h"

#include <QIODevice>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QLatin1String>

#include <QDebug>

Io * Io::m_instance = nullptr;

Io::~Io()
{
    m_instance = nullptr;
}

Io & Io::create()
{
    return instance();
}

Io & Io::instance()
{
    if (m_instance == nullptr) {
        m_instance = new Io();
        m_instance->initialize();
    }
    return * m_instance;
}

bool Io::created()
{
    return m_instance != nullptr;
}

void Io::initialize()
{
    QDir qdir;
    m_sep = qdir.separator();

    QString dirName = SailfishApp::pathToSharedDir().toString();
//    qdir.remove(dirName);
    qdir.mkpath(dirName);
    qdir.setPath(dirName);

    qDebug() << dirName;
    m_dir = qdir.path();
}

bool Io::loadJson(QString fName, QJsonObject &jsonObj)
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

bool Io::saveJson(QString fName, QJsonObject &jsonObj)
{
    QFile saveFile(fName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "saveJson" << "Couldn't open file" << fName;
        return false;
    }

    saveFile.write(QJsonDocument(jsonObj).toJson(QJsonDocument::Indented));
    return true;
}

bool Io::loadGame(QString fName, QByteArray &buffer)
{
    QFile loadFile(fName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qDebug() << "loadGame" << "Couldn't open file" << fName;
        return false;
    }

    buffer = loadFile.readAll();
    return true;
}

bool Io::saveGame(QString fName, QByteArray &buffer)
{
    QFile saveFile(fName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qDebug() << "saveGame" << "Couldn't open file" << fName;
        return false;
    }

    saveFile.write(buffer);
    return true;
}
