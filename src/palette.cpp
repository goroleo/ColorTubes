#include "palette.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#include "ctglobal.h"
#include "io.h"


Palette* Palette::m_instance = nullptr;

Palette& Palette::create()
{
    if (m_instance == nullptr) {
        m_instance = new Palette();
        m_instance->initialize();
    }
    return *m_instance;
}

Palette& Palette::instance()
{
    return *m_instance;
}

void Palette::initialize()
{
    m_items = new quint32[CtGlobal::NUM_OF_COLORS];
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        m_items[i] = 0;
    }
    if (!load())
    {
        setDefault();
        save();
    }
}

Palette::~Palette()
{
    save();
    delete[] m_items;
    delete m_instance;
    m_instance = nullptr;
}

int Palette::size()
{
    return CtGlobal::NUM_OF_COLORS;
}


QColor Palette::getColor(int index)
{
    if (index == 0) {
        return m_background;
    } else if ((index > 0) && (index <= CtGlobal::NUM_OF_COLORS)) {
        return QColor(m_items[index - 1]);
    } else
        return nullptr;
}

void Palette::setColor(int index, quint32 rgb)
{
    if ((index > 0) && (index <= CtGlobal::NUM_OF_COLORS)) {
        m_items[index - 1] = rgb;
    }
}

void Palette::setDefault()
{
    m_items[0] = 0xff38ff4d; // 1
    m_items[1] = 0xff1dd3f4; // 2
    m_items[2] = 0xff884822; // 3
    m_items[3] = 0xff8eaf00; // 4
    m_items[4] = 0xff737f8c; // 5
    m_items[5] = 0xff067606; // 6
    m_items[6] = 0xff3632de; // 7
    m_items[7] = 0xfff36d00; // 8
    m_items[8] = 0xffaf008f; // 9
    m_items[9] = 0xffe60f04; // 10
    m_items[10] = 0xffff7abc; // 11
    m_items[11] = 0xffffeb04; // 12
    m_background = QColor(0xff282828);
    m_dialog = QColor(0xff1c1127);
}

bool Palette::load()
{
    QJsonObject jObj;

    qDebug() << "Palette file" << CtGlobal::paletteFile();
    if (!CtGlobal::io().loadJson(CtGlobal::paletteFile(), jObj))
    {
        qDebug() << "Palette file load failure";
        return false;
    }
    qDebug() << "Palette file load success";

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
        result = result && count != CtGlobal::NUM_OF_COLORS;

        // process palette's colors
        int i = 0;
        while (result && i < count)
        {
            key = "color" + CtGlobal::intToStr(i+1);
            if (jPal.contains(key) && jPal[key].isString())
            {
                value = jPal[key].toString();
                if ((value.length() == 7) && value.startsWith("#"))
                {
                    m_items[i] = CtGlobal::colorStrToRgb(result, value);
                } else result = false;

            } else result = false;
            i++;
        }
    } // end process palette

    // process system colors
    result = result && jObj.contains("colors") && jObj["colors"].isObject();
    if (result) {
        QJsonObject jColors = jObj["colors"].toObject();

        // background color
        if (jColors.contains("background") && jColors["background"].isString())
        {
            value = jColors["background"].toString();
            if ((value.length() == 7) && value.startsWith("#"))
            {
                m_background = QColor(CtGlobal::colorStrToRgb(result, value));
            } else result = false;
        } else result = false;

        // dialog color
        if (result && jColors.contains("dialog") && jColors["dialog"].isString())
        {
            value = jColors["dialog"].toString();
            if ((value.length() == 7) && value.startsWith("#"))
            {
                m_dialog = QColor(CtGlobal::colorStrToRgb(result, value));
            } else result = false;
        } else result = false;
    }

    qDebug() << "Palette file load result" << result;
    return result;
}

bool Palette::save()
{
    // creates json
    QJsonObject jItem;
    QJsonObject jObj;

    // adds count
    jItem["count"] = CtGlobal::intToStr(CtGlobal::NUM_OF_COLORS);

    // adds colors
    for (int i = 0; i < CtGlobal::NUM_OF_COLORS; i++)
    {
        jItem["color"+CtGlobal::intToStr(i+1)] = CtGlobal::colorRgbToStr(m_items[i]);
    }
    jObj["palette"] = jItem;

    jItem = QJsonObject();

    // adds system colors
    jItem["background"] = CtGlobal::colorRgbToStr(m_background.rgb());
    jItem["dialog"] = CtGlobal::colorRgbToStr(m_dialog.rgb());;
    jObj["colors"] = jItem;

    // save
    bool result = CtGlobal::io().saveJson(CtGlobal::paletteFile(), jObj);
    qDebug() << "Palette save result" << result;

    return result;
}

