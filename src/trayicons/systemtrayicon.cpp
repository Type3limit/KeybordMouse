#include "systemtrayicon.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QLayout>
#include <QScreen>
#include <QLayout>
#include <QApplication>
#include <QDebug>
#include <qhotkey.h>

#include "dialogs/fullscreenareawindow.h"
#include "dialogs/settingwindow.h"
#include "dialogs/usermessagebox.h"
#include "helpers/globalsignal.h"
#include "helpers/optionnalchain.h"
#ifdef WIN32
#include "windows.h"
#include "shellapi.h"
Q_GUI_EXPORT HICON qt_pixmapToWinHICON(const QPixmap&);
#endif

SystemTrayIcon::SystemTrayIcon(QWidget* parent)
    : QWidget(parent)
    , m_systemTrayIcon(new QSystemTrayIcon(this))
    , m_menu(new ElaMenu())
{
    connect(m_systemTrayIcon, &QSystemTrayIcon::activated,
            this, [&](QSystemTrayIcon::ActivationReason reason)
            {
                if (reason == QSystemTrayIcon::DoubleClick)
                {
                    emit GlobalSignal::instance()->requestOpenSettingWindow();
                }
                if (reason == QSystemTrayIcon::Trigger)
                {
                    emit GlobalSignal::instance()->requestOpenFullScreenWindow();
                }
                if (reason == QSystemTrayIcon::Context)
                {
                    m_menu->exec(QCursor::pos());
                }
            });

    //empty icon
    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::transparent);
    m_blankIcon = QIcon(pixmap);
    //normal icon
    m_normalIcon = QIcon(":/icons/images/keybord.svg");

    m_systemTrayIcon->setIcon(m_normalIcon);

    m_systemTrayIcon->setToolTip(QStringLiteral("KeybordMouse"));

    setMouseTracking(true);


    initConfig();
    initWindow();
    initSignals();
    initMenus();

    m_systemTrayIcon->show();
}

SystemTrayIcon::~SystemTrayIcon()
{
    if (m_menu)
    {
        m_menu->deleteLater();
    }
    if (m_fullScreenAreaWindow)
    {
        m_fullScreenAreaWindow->deleteLater();
    }
    if (m_settingWindow)
    {
        m_settingWindow->deleteLater();
    }
}

void SystemTrayIcon::showIcon()
{

    m_systemTrayIcon->show();

}

void SystemTrayIcon::hideIcon()
{

    m_systemTrayIcon->hide();

}


void SystemTrayIcon::initConfig()
{
    m_config.showFullScreenHotKey;
    QHotkey *hotkey = new QHotkey(QKeySequence(m_config.showFullScreenHotKey), true, this);
    connect(hotkey,&QHotkey::activated,this,[&]()
    {
        emit GlobalSignal::instance()->requestOpenFullScreenWindow();
    });
}

void SystemTrayIcon::initWindow()
{
    m_fullScreenAreaWindow = new FullScreenAreaWindow(nullptr);
    m_fullScreenAreaWindow->setAttribute(Qt::WA_DeleteOnClose,false);
    m_settingWindow = new SettingWindow(nullptr);
    m_settingWindow->setAttribute(Qt::WA_DeleteOnClose,false);
}

void SystemTrayIcon::initMenus()
{
    m_acts[ShowWindow] = new QAction(QStringLiteral("显示窗口"), m_menu);
    m_acts[ShowWindow]->setIcon(QIcon(":/icons/images/Window.svg"));
    connect(m_acts[ShowWindow], &QAction::triggered, this, [&]()
    {
        emit GlobalSignal::instance()->requestOpenFullScreenWindow();
    });
    m_menu->addAction(m_acts[ShowWindow]);

    m_acts[ShowSettingWindow] = new QAction(QStringLiteral("设置"), m_menu);
    m_acts[ShowSettingWindow]->setIcon(QIcon(":/icons/images/setting.svg"));
    connect(m_acts[ShowSettingWindow], &QAction::triggered, this, [&]()
    {
        emit GlobalSignal::instance()->requestOpenSettingWindow();
    });
    m_menu->addAction(m_acts[ShowSettingWindow]);

    m_menu->addSeparator();

    m_acts[Quit] = new QAction(QStringLiteral("退出"), m_menu);
    m_acts[Quit]->setIcon(QIcon(":/icons/images/close.svg"));
    connect(m_acts[Quit], &QAction::triggered, this, [&]()
    {
        emit GlobalSignal::instance()->requestQuit();
    });
    m_menu->addAction(m_acts[Quit]);

    m_menu->setLayout(new QVBoxLayout(m_menu));

    m_systemTrayIcon->setContextMenu(m_menu);
}

void SystemTrayIcon::initSignals()
{
    connect(GlobalSignal::instance(), &GlobalSignal::requestQuit, this, [&]()
    {
        if (UserMessageBox::ButtonType::Ok == UserMessageBox::question(nullptr, u8"即将退出", u8"确认退出吗？"))
        {
            qApp->quit();
        }
    });
    connect(GlobalSignal::instance(),&GlobalSignal::requestShowMessage,this,[&](const QString& message)
    {
        m_systemTrayIcon->showMessage(u8"KeybordMouse",message);
    });
    connect(GlobalSignal::instance(),&GlobalSignal::requestOpenSettingWindow,this,[&]()
    {
       if (m_settingWindow)
       {
           m_settingWindow->moveToCenter();
           m_settingWindow->show();
       }
    });
    connect(GlobalSignal::instance(),&GlobalSignal::requestOpenFullScreenWindow,this,[&]()
    {
        if (m_fullScreenAreaWindow)
        {
            qDebug()<<"try invoke full screen window";
            QScreen* primary = QGuiApplication::primaryScreen();
            auto screenInUse = from(QGuiApplication::screens()).firstOf([&](QScreen* scr)
            {
                return scr->geometry().contains(QCursor::pos());
            },primary).get();
            qDebug()<<"full screen window start in screen:"<<screenInUse->name();
            m_fullScreenAreaWindow->setGeometry(screenInUse->geometry());
            qDebug()<<"full screen window with geometry:" << screenInUse->geometry();
            // m_fullScreenAreaWindow->raise();
            // m_fullScreenAreaWindow->activateWindow();
            m_fullScreenAreaWindow->hide();
            m_fullScreenAreaWindow->showNormal();
            m_fullScreenAreaWindow->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
            qDebug()<<"invoke full screen window finished";
        }
        else
        {
            qDebug()<<"full screen window not exist";
        }
    },Qt::QueuedConnection);
}

void SystemTrayIcon::updateTrayIconTip(const QString& tip)
{

    m_systemTrayIcon->setToolTip(tip);
}

void SystemTrayIcon::setIcon(QIcon ico)
{
    m_systemTrayIcon->setIcon(ico);
}
