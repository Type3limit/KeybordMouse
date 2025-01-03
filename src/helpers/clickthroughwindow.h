//
// Created by 58226 on 2025/1/3.
//

#ifndef CLICKTHROUGHWINDOW_H
#define CLICKTHROUGHWINDOW_H


#include <QWidget>

class ClickThroughWindow {
public:
    static bool enableClickThrough(QWidget* window);
    static bool disableClickThrough(QWidget* window);

private:
#if defined(Q_OS_WIN)
    static bool enableClickThroughWindows(QWidget* window);
    static bool disableClickThroughWindows(QWidget* window);
#elif defined(Q_OS_LINUX)
    static bool enableClickThroughLinux(QWidget* window);
    static bool disableClickThroughLinux(QWidget* window);
#elif defined(Q_OS_MAC)
    static bool enableClickThroughMac(QWidget* window);
    static bool disableClickThroughMac(QWidget* window);
#endif
};

#endif //CLICKTHROUGHWINDOW_H
