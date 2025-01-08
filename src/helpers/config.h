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
#include <QHash>

#define CURRENT_PROGRAM_VERSION  QString("1.0.0")
#define STR(NAME) QStringLiteral(#NAME)
#define KEY_STRV(NAME) {NAME, STR(NAME)}
using StrEx = ExtensionMethods::QStringExtension;

enum ScreenOptionMode
{
    QWER = 0,
    ABCD
};

const QStringList ScreenOptionModeDisplayStr = {
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

enum ConfigType
{
    ShowFullScreenHotKey = 0,
    CloseWindowHotKey,
    CancelSelectionHotKey,
    SwitchScreenHotKey,
    LeftButtonClickHotKey,
    RightButtonClickHotKey,
    DragHotKey,
    QuickLeftDoubleClickHotKey,
    QuickLeftTripleClickHotKey,
    QuickRightDoubleClickHotKey,
    QuickRightTripleClickHotKey,

    MoveColumnLeftHotKey,
    MoveColumnRightHotKey,
    MoveRowUpHotKey,
    MoveRowDownHotKey,

    MiddleButtonClickHotKey,
    ForwardButtonClickHotKey,
    BackwardButtonClickHotKey,

    OpenSettingWindowHotKey,
    CloseSettingWindowHotKey,

    n_ConfigType
};

class Config
{
public:
    static Config* instance();

    bool isDefault() const;

    QList<QKeySequence> getHotkeySequence(ConfigType type) const;

    QString getHotkeyDisplayName(ConfigType type) const;

    void addHotkeySequence(ConfigType type, QKeySequence sequence);

    void setHotkeySequence(ConfigType type, QList<QKeySequence> sequences);

    void resetHotKeySequence(ConfigType type);

    void setOptionMode(ScreenOptionMode mode);

    ScreenOptionMode getOptionMode() const;

    void setSubCellOptionMode(ScreenOptionMode mode);

    ScreenOptionMode getSubCellOptionMode() const;

    void setCloseAfterClick(bool value);

    bool getCloseAfterClick() const;

    void setCloseAfterDrag(bool value);

    bool getCloseAfterDrag() const;

    void writeJson();

    void readJson();

    Config& operator=(Config& other);

private:
    const QString ConfigPath = "./config.json";

    QHash<ConfigType, QList<QKeySequence>> hotkeySequence = {

        {ShowFullScreenHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_S)}},

        {CloseWindowHotKey, {QKeySequence(Qt::AltModifier|Qt::Key_Escape)}},

        {CancelSelectionHotKey, {QKeySequence(Qt::Key_Backspace)}},

        {
            SwitchScreenHotKey,
            {QKeySequence(Qt::AltModifier | Qt::Key_Right), QKeySequence(Qt::AltModifier | Qt::Key_Left)}
           },

        {LeftButtonClickHotKey, {QKeySequence(Qt::Key_Space)}},

        {RightButtonClickHotKey, {QKeySequence(Qt::ShiftModifier | Qt::Key_Space)}},

        {DragHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_Space)}},

        {QuickLeftDoubleClickHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_D)}},

        {QuickLeftTripleClickHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_T)}},

        {QuickRightDoubleClickHotKey, {QKeySequence((Qt::ShiftModifier | Qt::AltModifier) | Qt::Key_D)}},

        {QuickRightTripleClickHotKey, {QKeySequence((Qt::ShiftModifier | Qt::AltModifier) | Qt::Key_T)}},

        {MoveColumnLeftHotKey, {QKeySequence(Qt::Key_Left)}},

        {MoveColumnRightHotKey, {QKeySequence(Qt::Key_Right)}},

        {MoveRowUpHotKey, {QKeySequence(Qt::Key_Up)}},

        {MoveRowDownHotKey, {QKeySequence(Qt::Key_Down)}},

        {MiddleButtonClickHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_M)}},

        {ForwardButtonClickHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_Right)}},

        {BackwardButtonClickHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_Left)}},

        {OpenSettingWindowHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_O)}},

        {CloseSettingWindowHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_C)}}
    };


    QHash<ConfigType, QList<QKeySequence>> defaultHotkeySequence = {

        {ShowFullScreenHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_S)}},

        {CloseWindowHotKey, {QKeySequence(Qt::Key_Escape)}},

        {CancelSelectionHotKey, {QKeySequence(Qt::Key_Backspace)}},

        {
            SwitchScreenHotKey,
            {QKeySequence(Qt::AltModifier | Qt::Key_Right), QKeySequence(Qt::AltModifier | Qt::Key_Left)}
           },

        {LeftButtonClickHotKey, {QKeySequence(Qt::Key_Space)}},

        {RightButtonClickHotKey, {QKeySequence(Qt::ShiftModifier | Qt::Key_Space)}},

        {DragHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_Space)}},

        {QuickLeftDoubleClickHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_D)}},

        {QuickLeftTripleClickHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_T)}},

        {QuickRightDoubleClickHotKey, {QKeySequence((Qt::ShiftModifier | Qt::AltModifier) | Qt::Key_D)}},

        {QuickRightTripleClickHotKey, {QKeySequence((Qt::ShiftModifier | Qt::AltModifier) | Qt::Key_T)}},

        {MoveColumnLeftHotKey, {QKeySequence(Qt::Key_Left)}},

        {MoveColumnRightHotKey, {QKeySequence(Qt::Key_Right)}},

        {MoveRowUpHotKey, {QKeySequence(Qt::Key_Up)}},

        {MoveRowDownHotKey, {QKeySequence(Qt::Key_Down)}},

        {MiddleButtonClickHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_M)}},

        {ForwardButtonClickHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_Right)}},

        {BackwardButtonClickHotKey, {QKeySequence(Qt::ControlModifier | Qt::Key_Left)}},

        {OpenSettingWindowHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_O)}},

        {CloseSettingWindowHotKey, {QKeySequence(Qt::AltModifier | Qt::Key_C)}}
    };


    const QHash<ConfigType, QString> hotkeyStr = {
        KEY_STRV(ShowFullScreenHotKey),
        KEY_STRV(CloseWindowHotKey),
        KEY_STRV(CancelSelectionHotKey),
        KEY_STRV(SwitchScreenHotKey),
        KEY_STRV(LeftButtonClickHotKey),
        KEY_STRV(RightButtonClickHotKey),
        KEY_STRV(DragHotKey),
        KEY_STRV(QuickLeftDoubleClickHotKey),
        KEY_STRV(QuickLeftTripleClickHotKey),
        KEY_STRV(QuickRightDoubleClickHotKey),
        KEY_STRV(QuickRightTripleClickHotKey),
        KEY_STRV(MoveColumnLeftHotKey),
        KEY_STRV(MoveColumnRightHotKey),
        KEY_STRV(MoveRowUpHotKey),
        KEY_STRV(MoveRowDownHotKey),
        KEY_STRV(MiddleButtonClickHotKey),
        KEY_STRV(ForwardButtonClickHotKey),
        KEY_STRV(BackwardButtonClickHotKey),
        KEY_STRV(OpenSettingWindowHotKey),
        KEY_STRV(CloseSettingWindowHotKey)
    };

    const QHash<ConfigType, QString> hotkeyDisplayStr = {
        {ShowFullScreenHotKey, u8"显示窗口"},
        {CloseWindowHotKey, u8"关闭窗口"},
        {CancelSelectionHotKey, u8"取消选择"},
        {SwitchScreenHotKey, u8"切换屏幕"},
        {LeftButtonClickHotKey, u8"左键单击"},
        {RightButtonClickHotKey, u8"右键单击"},
        {DragHotKey, u8"拖拽"},
        {QuickLeftDoubleClickHotKey, u8"快速左键双击"},
        {QuickLeftTripleClickHotKey, u8"快速左键三击"},
        {QuickRightDoubleClickHotKey, u8"快速右键双击"},
        {QuickRightTripleClickHotKey, u8"快速右键三击"},
        {MoveColumnLeftHotKey, u8"向左移动"},
        {MoveColumnRightHotKey, u8"向右移动"},
        {MoveRowUpHotKey, u8"向上移动"},
        {MoveRowDownHotKey, u8"向下移动"},
        {MiddleButtonClickHotKey, u8"中键单击"},
        {ForwardButtonClickHotKey, u8"前进"},
        {BackwardButtonClickHotKey, u8"后退"},
        {OpenSettingWindowHotKey,u8"开启设置窗口"},
        {CloseSettingWindowHotKey,u8"关闭设置窗口"}
    };


    ScreenOptionMode optionMode = ScreenOptionMode::QWER;

    ScreenOptionMode subCellMode = ScreenOptionMode::QWER;

    bool closeAfterClick = true;

    bool closeAfterDrag = true;

    Config();

    static Config s_instance;
};


#define UPDATE_FILE_NAME "/ProgramInstaller.exe"

#define UPDATE_FILE_LOCAL_NAME QApplication::applicationDirPath() + UPDATE_FILE_NAME

#endif //CONFIG_H
