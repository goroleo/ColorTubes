#include "options.h"
#include <QJsonObject>
#include <QtCore>
#include "src/ctglobal.h"
#include "src/ctio.h"


Options::Options()
{
    if (!load()) {
        level = 0;
    }
}

Options::~Options()
{
    save();
}

bool Options::load()
{

    QJsonObject jObj;

    if (!CtGlobal::io().loadJson(CtGlobal::settingsFileName(), jObj))
        return false;

    bool result = jObj.contains("settings") && jObj["settings"].isObject();
    if (result) {
        QJsonObject jSettings = jObj["settings"].toObject();

        // level
        if (jSettings.contains("lastLevel") && jSettings["lastLevel"].isString())
            level = jSettings["lastLevel"].toString().toInt(&result, 10);
        result = level > 0;
    }

    if (result)
        qDebug() << "Settings loaded from" << CtGlobal::settingsFileName();
    return result;
}

bool Options::save()
{
    // creates json
    QJsonObject jItem;
    QJsonObject jObj;

    // adds Level
    jItem["lastLevel"] = CtGlobal::intToStr(level);

    jObj["settings"] = jItem;

    // save
    qDebug() << "Settings saves to" << CtGlobal::settingsFileName();
    return CtGlobal::io().saveJson(CtGlobal::settingsFileName(), jObj);
}

