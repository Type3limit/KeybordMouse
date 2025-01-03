#include "systemtrayicon.h"
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QDesktopWidget>
#include <QLayout>
#include <QScreen>
#include <QLayout>
#include <QApplication>
#include <QDebug>
#include <qhotkey.h>

#include "traypos.h"
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
    , m_trayPos(new MsgTrayPos())
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

    m_systemTrayIcon->setToolTip(QStringLiteral("屏幕录制"));

    setMouseTracking(true);

#ifdef WIN32
    m_iconPtr = new NOTIFYICONDATAW();
    createTrayIcon();
    m_trayPos->SetNotifyIconInfo(HWND(this->winId()), 1, WM_TRAYNOTIFY);
#else
    m_systemTrayIcon->show();
#endif
    initConfig();
    initWindow();
    initSignals();
    initMenus();
}

SystemTrayIcon::~SystemTrayIcon()
{
    if (m_menu)
    {
        m_menu->deleteLater();
    }

#ifdef WIN32
    if (m_iconPtr)
    {
        delete static_cast<NOTIFYICONDATAW*>(m_iconPtr);
    }
    removeTrayIcon();
#endif
}

void SystemTrayIcon::showIcon()
{
#ifdef WIN32
    createTrayIcon();
#else
    m_systemTrayIcon->show();
#endif
}

void SystemTrayIcon::hideIcon()
{
#ifdef WIN32
    removeTrayIcon();
#else
    m_systemTrayIcon->hide();
#endif
}

#ifdef WIN32

bool SystemTrayIcon::nativeEventFilter(const QByteArray& eventType, void* message, long* result)
{
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG* pMsg = reinterpret_cast<MSG*>(message);
        if (pMsg->message == WM_TRAYNOTIFY)
        {
            switch (pMsg->lParam)
            {
            case WM_MOUSEMOVE:
                {
                    m_trayPos->OnMouseMove();
                    break;
                }

            case WM_MOUSEHOVER:
                {
                    break;
                }
            case WM_MOUSELEAVE:
                {
                    break;
                }
            case WM_LBUTTONDBLCLK:
                {
                    emit GlobalSignal::instance()->requestOpenSettingWindow();
                    break;
                }
            case WM_LBUTTONDOWN:
                {
                    emit GlobalSignal::instance()->requestOpenFullScreenWindow();
                    break;
                }
            case WM_RBUTTONDOWN:
                {
                    m_menu->exec(QCursor::pos());
                    break;
                }
            }
        }
    }

    return false;
}

void SystemTrayIcon::createTrayIcon()
{
    NOTIFYICONDATAW nid = *((NOTIFYICONDATAW*)m_iconPtr);
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.uID = 1;
    nid.hWnd = (HWND)winId(); // 获取 QWidget 窗口句柄
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYNOTIFY;
    nid.hIcon = qt_pixmapToWinHICON(m_normalIcon.pixmap(16, 16));
    wcscpy_s(nid.szTip, L"屏幕录制");

    Shell_NotifyIconW(NIM_ADD, &nid);
    // 启动鼠标悬浮事件
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = nid.hWnd;
    tme.dwHoverTime = 1; // 设置为非常短的时间来激活 hover 事件
    TrackMouseEvent(&tme);
    DestroyIcon(nid.hIcon);
}

void SystemTrayIcon::removeTrayIcon()
{
    NOTIFYICONDATAW nid = *((NOTIFYICONDATAW*)m_iconPtr);
    Shell_NotifyIconW(NIM_DELETE, &nid);
}

#endif

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
    m_fullScreenAreaWindow = new FullScreenAreaWindow(this);
    m_fullScreenAreaWindow->setAttribute(Qt::WA_DeleteOnClose,false);
    m_settingWindow = new SettingWindow(this);
    m_settingWindow->setAttribute(Qt::WA_DeleteOnClose,false);
}

void SystemTrayIcon::initMenus()
{
    m_acts[ShowWindow] = new QAction(QStringLiteral("显示窗口"), m_menu);
    m_acts[ShowWindow]->setIcon(QIcon(":/icons/images/areas.svg"));
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
            QScreen* primary = QGuiApplication::primaryScreen();
            auto screenInUse = from(QGuiApplication::screens()).firstOf([&](QScreen* scr)
            {
                return scr->geometry().contains(QCursor::pos());
            },primary).get();

            m_fullScreenAreaWindow->setGeometry(screenInUse->geometry());
            m_fullScreenAreaWindow->setFocus();
            m_fullScreenAreaWindow->exec();
        }
    });
}

void SystemTrayIcon::updateTrayIconTip(const QString& tip)
{
#ifdef WIN32
    NOTIFYICONDATAW nid = *((NOTIFYICONDATAW*)m_iconPtr);
    wcscpy_s(nid.szTip, tip.toStdWString().data());
    Shell_NotifyIconW(NIM_MODIFY, &nid);
#else
    m_systemTrayIcon->setToolTip(tip);
#endif
}

void SystemTrayIcon::setIcon(QIcon ico)
{
#ifdef WIN32
    NOTIFYICONDATAW nid = *((NOTIFYICONDATAW*)m_iconPtr);
    nid.hIcon = qt_pixmapToWinHICON(QIcon(ico).pixmap(16, 16));
    Shell_NotifyIconW(NIM_MODIFY, &nid);
    DestroyIcon(nid.hIcon);
#else
    m_systemTrayIcon->setIcon(ico);
#endif
}
