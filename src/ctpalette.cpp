#include "ctpalette.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "ctglobal.h"
#include "ctio.h"

CtPalette * CtPalette::m_instance = nullptr;

CtPalette& CtPalette::create()
{
    return instance();
}

CtPalette& CtPalette::instance()
{
    if (m_instance == nullptr) {
        m_instance = new CtPalette();
        m_instance->initialize();
        qDebug() << "Palette created";
    }
    return *m_instance;
}

void CtPalette::initialize()
{
    m_items = new quint32[CT_NUM_OF_COLORS];
    for (int i = 0; i < CT_NUM_OF_COLORS; i++) {
        m_items[i] = 0;
    }
    if (!load()) {
        setDefault();
        save();
    }
}

CtPalette::~CtPalette()
{
    qDebug() << "Palette destroyed";
    save();
    delete[] m_items;
    m_instance = nullptr;
}

int CtPalette::size()
{
    return CT_NUM_OF_COLORS;
}

QColor CtPalette::getColor(int index)
{
    if (index == 0) {
        return QColor((quint32) 0);
    } else if ((index > 0) && (index <= CT_NUM_OF_COLORS)) {
        return QColor(m_items[index - 1]);
    } else
        return nullptr;
}

void CtPalette::setColor(int index, quint32 rgb)
{
    if ((index > 0) && (index <= CT_NUM_OF_COLORS)) {
        m_items[index - 1] = rgb;
    }
}

void CtPalette::setDefault()
{
    m_items[0]  = 0xff38ff4d;
    m_items[1]  = 0xff1dd3f4;
    m_items[2]  = 0xff884822;
    m_items[3]  = 0xff8eaf00;
    m_items[4]  = 0xff737f8c;
    m_items[5]  = 0xff067606;
    m_items[6]  = 0xff3632de;
    m_items[7]  = 0xfff36d00;
    m_items[8]  = 0xffaf008f;
    m_items[9]  = 0xffe60f04;
    m_items[10] = 0xffff7abc;
    m_items[11] = 0xffffeb04;
}

bool CtPalette::load()
{
    QJsonObject jObj;

    if (!CtGlobal::io().loadJson(CtGlobal::paletteFileName(), jObj))
        return false;

    int count = 0;
    QString key;
    QString value;

    // process palette
    bool result = jObj.contains("palette") && jObj["palette"].isObject();
    if (result) {
        QJsonObject jPal = jObj["palette"].toObject();

        // check count
        if (jPal.contains("count") && jPal["count"].isString())
            count = jPal["count"].toString().toInt(&result, 10);
        result = result && count == CT_NUM_OF_COLORS;

        // process palette's colors
        int i = 0;
        while (result && i < count) {
            key = "color" + CtGlobal::intToStr(i+1);
            if (jPal.contains(key) && jPal[key].isString()) {
                value = jPal[key].toString();
                if ((value.length() == 7) && value.startsWith("#"))
                    m_items[i] = CtGlobal::colorStrToRgb(result, value);
                else
                    result = false;
            } else
                result = false;
            i++;
        }
    } // end process palette

    if (result)
        qDebug() << "Palette loaded from" << CtGlobal::paletteFileName();
    return result;
}

bool CtPalette::save()
{
    // creates json
    QJsonObject jItem;
    QJsonObject jObj;

    // adds count
    jItem["count"] = CtGlobal::intToStr(CT_NUM_OF_COLORS);

    // adds colors
    for (int i = 0; i < CT_NUM_OF_COLORS; i++) {
        jItem["color"+CtGlobal::intToStr(i+1)] = CtGlobal::colorRgbToStr(m_items[i]);
    }
    jObj["palette"] = jItem;

    // save
    qDebug() << "Saving palette to" << CtGlobal::paletteFileName();
    return CtGlobal::io().saveJson(CtGlobal::paletteFileName(), jObj);
}

