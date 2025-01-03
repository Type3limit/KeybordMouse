#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H


#include <QIcon>
#include <QWidget>
#include <QAction>
#include <QSystemTrayIcon>
#include "ElaMenu.h"
#include "helpers/config.h"


#ifdef WIN32
#include <QAbstractNativeEventFilter>
class MsgTrayPos;
#endif
class QSystemTrayIcon;
class FullScreenAreaWindow;
class SettingWindow;
class SystemTrayIcon : public QWidget
#ifdef WIN32
    ,virtual public QAbstractNativeEventFilter
#endif
{
    Q_OBJECT
public:

    enum MenuActionTypes
    {
        ShowWindow=0,
        ShowSettingWindow,
        Quit,
        n_ActionTypes
    };
    explicit SystemTrayIcon(QWidget *parent = nullptr);
    ~SystemTrayIcon() override;

    void showIcon();
    void hideIcon();

#ifdef WIN32
    bool nativeEventFilter(const QByteArray & eventType, void * message, long * result) override;
    void createTrayIcon();
    void removeTrayIcon();
    void initConfig();
    void initWindow();
    void initMenus();
    void initSignals();
#endif
    void setIcon(QIcon ico);
    void updateTrayIconTip(const QString& tip);

private:
    QSystemTrayIcon *m_systemTrayIcon;
    ElaMenu *m_menu;
    QAction* m_acts [n_ActionTypes];
    volatile bool m_iconFlag{ false};
    QIcon m_normalIcon;
    QIcon m_blankIcon;
    Config m_config;
    bool m_isRecordingStarted = false;
    bool m_isPaused= false;
#ifdef WIN32
    void * m_iconPtr{nullptr};
    MsgTrayPos* m_trayPos{nullptr};
#endif

    FullScreenAreaWindow* m_fullScreenAreaWindow{nullptr};
    SettingWindow* m_settingWindow{nullptr};
};

#endif // SYSTEMTRAYICON_H
