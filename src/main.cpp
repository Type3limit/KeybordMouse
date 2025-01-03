//
// Created by 58226 on 2025/1/2.
//
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QSharedMemory>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTcpSocket>
#include <QTcpServer>

#include "ElaApplication.h"
#include "helpers/config.h"
#include "helpers/debug.h"
#include "helpers/messages.h"
#include "trayicons/systemtrayicon.h"

#ifdef _WIN32
#ifndef NOMINMAX
# define NOMINMAX
#endif
#include <windows.h>
#include <dbghelp.h>

inline LONG WINAPI exceptionCallback(struct _EXCEPTION_POINTERS* exceptionInfo)
{
    QCoreApplication *app = QApplication::instance();

    QString savePath = app->applicationDirPath() + "/dump/";
    qDebug()<<"save path :"<<savePath;
    QDir dir(savePath);
    if (!dir.exists() && !dir.mkpath(savePath)) {
        app->exit(E_UNEXPECTED);
        return EXCEPTION_EXECUTE_HANDLER;
    }
    savePath.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
    savePath.append(".dmp");

    HANDLE dump = CreateFileW(savePath.toStdWString().c_str(), GENERIC_WRITE,
                              0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == dump) {
        app->exit(E_UNEXPECTED);
        return EXCEPTION_EXECUTE_HANDLER;
    }

    MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInfo;
    miniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
    miniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
    miniDumpExceptionInfo.ClientPointers = TRUE;
    DWORD idProcess = GetCurrentProcessId();
    MiniDumpWriteDump(GetCurrentProcess(), idProcess, dump,
                      MiniDumpNormal/*(MINIDUMP_TYPE)flags*/, &miniDumpExceptionInfo, NULL, NULL);

    CloseHandle(dump);

    app->exit(E_UNEXPECTED);
    return EXCEPTION_EXECUTE_HANDLER;
}
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <QTranslator>
#include <QLibraryInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QThread>
#include "dialogs/usermessagebox.h"

int main(int argc, char* argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Round);
#endif
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));

    QApplication app(argc, argv);

    app.setApplicationVersion(CURRENT_PROGRAM_VERSION);

    app.setApplicationName("KeybordMouse");

    app.setApplicationDisplayName(u8"屏幕键盘");

    eApp->init();

    QTcpSocket socket;
    socket.connectToHost("localhost",TCP_DEFAULT_PORT);
    // 尝试连接到服务器
    if (socket.waitForConnected(500)) {
        // 向已有实例发送激活窗口的消息
        if (socket.isWritable()) {
            socket.write(ACTIVE_WINDOW_MESSAGE);
            socket.waitForBytesWritten();
        }
        socket.close();
        return 0; // 退出当前实例
    }

    // 创建本地服务器

    QTcpServer localServer;

    if (!localServer.listen(QHostAddress::Any,TCP_DEFAULT_PORT))
        {
        UserMessageBox::critical(nullptr, "Error", "Unable to start tcp server.");
        return 0;
    }



    initializeLogger();

#ifdef _WIN32
    SetUnhandledExceptionFilter(exceptionCallback);
#endif

    // RecordingWindow window(server, token, port, nullptr);
    // window.show();

    // 监听来自其他实例的消息
    QObject::connect(&localServer, &QTcpServer::newConnection, [&]() {
        QTcpSocket *clientSocket = localServer.nextPendingConnection();
        if (clientSocket->waitForReadyRead()) {
            QString message = clientSocket->readAll();
            if (message == ACTIVE_WINDOW_MESSAGE) {
                //window.activeCurrentWindow(); // 激活窗口
            }
        }
        clientSocket->deleteLater();
    });

    SystemTrayIcon trayIcon;

#ifdef WIN32
    qApp->installNativeEventFilter(&trayIcon);
#endif
    QApplication::setQuitOnLastWindowClosed(false);
    int code =  QApplication::exec();
    localServer.close();
#ifdef WIN32
    _CrtDumpMemoryLeaks();
#endif
    return code;
}
