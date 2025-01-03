//
// Created by 58226 on 2023/11/22.
//

#ifndef CONFIG_H
#define CONFIG_H
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "extensionmethods.h"
#include <QKeySequence>

#define CURRENT_PROGRAM_VERSION  QString("1.0.0")

using StrEx = ExtensionMethods::QStringExtension;

enum ConfigType
{
    ShowFullScreenHotKey = 0,
};

struct Config
{
public:
    QKeySequence showFullScreenHotKey = QKeySequence(Qt::SHIFT +Qt::Key_F7);
public:
    Config() { readJson(); }

    bool isDefault() const
    {
        return true;
    }

    const QString ConfigPath = "./config.json";

    void writeJson()
    {
        QJsonObject obj = {
            {"show_full_screen_shot_key", showFullScreenHotKey.toString()}
        };
        StrEx::writeAllText(ConfigPath, QJsonDocument(obj).toJson());
    }

    void readJson()
    {
        const auto doc = QJsonDocument::fromJson(StrEx::readAllText(ConfigPath).toUtf8());
        if (doc.isEmpty())
            return;
        showFullScreenHotKey.fromString(doc.object().value("show_full_screen_shot_key").toString());
    }

    Config& operator=(const Config& other)
    {
        showFullScreenHotKey = other.showFullScreenHotKey;
        return *this;
    }
};


#define UPDATE_FILE_NAME "/ProgramInstaller.exe"

#define UPDATE_FILE_LOCAL_NAME QApplication::applicationDirPath() + UPDATE_FILE_NAME

enum ScreenOptionMode
{
    QWER = 0,
    ABCD
};

const QStringList ScreenOptionModeDisplayStr[2] = {
    {"QWERUIOPASDFJKL;ZXCVM,./"},
    {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"}
};


const QStringList ScreenOptionModeStr[2] = {
    {
        "Q", "W", "E", "R", "U", "I", "O", "P",
        "A", "S", "D", "F", "J", "K", "L", ";",
        "Z", "X", "C", "V", "M", ",", ".", "/"
    },
    {
        "A", "B", "C", "D", "E", "F",
        "G", "H", "I", "J", "K", "L",
        "M", "N", "O", "P", "Q", "R",
        "S", "T", "U", "V", "W", "X",
        "Y", "Z"
    }
};

const QList<Qt::Key> ScrrenOptionModeKey[2] =
{
    {
        Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P,
        Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_J, Qt::Key_K, Qt::Key_L, Qt::Key_Semicolon,
        Qt::Key_Z, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_M, Qt::Key_Comma, Qt::Key_Period, Qt::Key_Slash
    },
    {
        Qt::Key_A, Qt::Key_B, Qt::Key_C, Qt::Key_D, Qt::Key_E, Qt::Key_F,
        Qt::Key_G, Qt::Key_H, Qt::Key_I, Qt::Key_J, Qt::Key_K, Qt::Key_L,
        Qt::Key_M, Qt::Key_N, Qt::Key_O, Qt::Key_P, Qt::Key_Q, Qt::Key_R,
        Qt::Key_S, Qt::Key_T, Qt::Key_U, Qt::Key_V, Qt::Key_W, Qt::Key_X,
        Qt::Key_Y, Qt::Key_Z
    }
};

#endif //CONFIG_H
