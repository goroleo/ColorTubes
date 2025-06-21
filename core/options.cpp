#include "options.h"
#include <QJsonObject>
#include <QtCore>
#include "src/ctglobal.h"
#include "src/ctio.h"


Options::Options()
{
    if (!load()) {
        level = 0;
        theme = 1;
        orientation = 1;
        undoMode = 0;
    }

    qDebug() << "level" << level
             << "theme" << theme
             << "orientation" << orientation
             << "undo mode" << undoMode;
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

        if (jSettings.contains("theme") && jSettings["theme"].isString()) {
            QString themeStr = jSettings["theme"].toString();
            if (themeStr.compare("light", Qt::CaseInsensitive) == 0)
                theme = 2;
            else if (themeStr.compare("system", Qt::CaseInsensitive) == 0)
                theme = 0;
            else
                theme = 1;
        } else
            theme = 1;

        if (jSettings.contains("orientation") && jSettings["orientation"].isString()) {
            QString orientationStr = jSettings["orientation"].toString();
            if (orientationStr.compare("landscape", Qt::CaseInsensitive) == 0)
                orientation = 2;
            else if (orientationStr.compare("system", Qt::CaseInsensitive) == 0)
                orientation = 0;
            else
                orientation = 1;
        } else
            orientation = 1;

        if (jSettings.contains("undoMode") && jSettings["undoMode"].isString()) {
            QString orientationStr = jSettings["undoMode"].toString();
            if (orientationStr.compare("until_solve", Qt::CaseInsensitive) == 0)
                undoMode = 1;
            else
                undoMode = 0;
        } else
            undoMode = 0;
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

    // adds theme
    QString str;
    switch (theme) {
    case 0:
        str = QString("system");
        break;
    case 2:
        str = QString("light");
        break;
    default:
        str = QString("dark");
    }
    jItem["theme"] = str;

    // adds orientation
    switch (orientation) {
    case 0:
        str = QString("system");
        break;
    case 2:
        str = QString("landscape");
        break;
    default:
        str = QString("portrait");
    }
    jItem["orientation"] = str;

    // adds undoMode
    if (undoMode == 1)
        str = QString("until_solve");
    else
        str = QString("one_move");
    jItem["undoMode"] = str;

    jObj["settings"] = jItem;

    // save
    qDebug() << "Settings saves to" << CtGlobal::settingsFileName();
    return CtGlobal::io().saveJson(CtGlobal::settingsFileName(), jObj);
}

