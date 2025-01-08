//
// Created by 58226 on 2025/1/7.
//
#include "config.h"


Config Config::s_instance;

Config::Config()
{
    readJson();
}

Config* Config::instance()
{
    return &s_instance;
}

bool Config::isDefault() const
{
    return true;
}

QList<QKeySequence> Config::getHotkeySequence(ConfigType type) const
{
    return hotkeySequence[type];
}

QString Config::getHotkeyDisplayName(ConfigType type) const
{
    return hotkeyDisplayStr[type];
}

void Config::addHotkeySequence(ConfigType type, QKeySequence sequence)
{
    hotkeySequence[type].append(sequence);
}

void Config::setHotkeySequence(ConfigType type, QList<QKeySequence> sequences)
{
    hotkeySequence[type]=sequences;
}

void Config::resetHotKeySequence(ConfigType type)
{
    hotkeySequence[type] = defaultHotkeySequence[type];
}


void Config::setOptionMode(ScreenOptionMode mode)
{
    optionMode = mode;
}

ScreenOptionMode Config::getOptionMode() const
{
    return optionMode;
}

void Config::setSubCellOptionMode(ScreenOptionMode mode)
{
    subCellMode = mode;
}

ScreenOptionMode Config::getSubCellOptionMode() const
{
    return subCellMode;
}

void Config::setCloseAfterClick(bool value)
{
    closeAfterClick = value;
}

bool Config::getCloseAfterClick() const
{
    return closeAfterClick;
}

void Config::setCloseAfterDrag(bool value)
{
    closeAfterDrag = value;
}

bool Config::getCloseAfterDrag() const
{
    return closeAfterDrag;
}

void Config::writeJson()
{
    QJsonObject obj;
    for (int i = 0 ;i<n_ConfigType;i++)
    {
        QJsonArray arr;
        auto strs = hotkeySequence[static_cast<ConfigType>(i)];
        for (const auto& str:strs)
        {
            arr.append(str.toString());
        }
        obj[hotkeyStr[static_cast<ConfigType>(i)]] = arr;
    }
    obj[STR(SubCellMode)] = subCellMode;
    obj[STR(OptionMode)] = optionMode;
    obj[STR(CloseAfterClick)] = closeAfterClick;
    obj[STR(CloseAfterDrag)] = closeAfterDrag;
    StrEx::writeAllText(ConfigPath, QJsonDocument(obj).toJson());
}

 void Config::readJson()
{
    const auto doc =
        QJsonDocument::fromJson(StrEx::readAllText(ConfigPath).toUtf8());
    if (doc.isEmpty())
        return;
    auto obj = doc.object();

    subCellMode = static_cast<ScreenOptionMode>(obj[STR(SubCellMode)].toInt());

    optionMode = static_cast<ScreenOptionMode>(obj[STR(OptionMode)].toInt());

    closeAfterClick = obj[STR(CloseAfterClick)].toBool();

    closeAfterDrag = obj[STR(CloseAfterDrag)].toBool();

    for (int i=0;i<n_ConfigType;i++)
    {
        QList<QKeySequence> sequences;
        auto arr = obj.value(hotkeyStr[static_cast<ConfigType>(i)]).toArray();
        for (const auto itr:arr)
        {
           sequences.append(QKeySequence(itr.toString()));
        }
        setHotkeySequence(static_cast<ConfigType>(i),sequences);
    }
}

 Config& Config::operator=(Config& other)
{
    hotkeySequence=QHash(other.hotkeySequence);
    subCellMode = other.subCellMode;
    optionMode = other.optionMode;
    closeAfterClick = other.closeAfterClick;
    closeAfterDrag = other.closeAfterDrag;
    return *this;
}
