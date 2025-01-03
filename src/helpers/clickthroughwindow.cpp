//
// Created by 58226 on 2025/1/3.
//

#include "clickthroughwindow.h"
#if defined(Q_OS_WIN)
    #include <windows.h>
#elif defined(Q_OS_LINUX)
    #include <QX11Info>
    #include <X11/Xlib.h>
    #include <X11/extensions/shape.h>
#elif defined(Q_OS_MAC)
    #include <Cocoa/Cocoa.h>
#endif

bool ClickThroughWindow::enableClickThrough(QWidget* window) {
    if (!window) return false;

    // Set common Qt properties
    window->setWindowFlags(window->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    window->setAttribute(Qt::WA_TranslucentBackground);
    window->setAttribute(Qt::WA_TransparentForMouseEvents);

    #if defined(Q_OS_WIN)
        return enableClickThroughWindows(window);
    #elif defined(Q_OS_LINUX)
        return enableClickThroughLinux(window);
    #elif defined(Q_OS_MAC)
        return enableClickThroughMac(window);
    #else
        return true;
    #endif
}

bool ClickThroughWindow::disableClickThrough(QWidget* window) {
    if (!window) return false;

    // Reset common Qt properties
    window->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    #if defined(Q_OS_WIN)
        return disableClickThroughWindows(window);
    #elif defined(Q_OS_LINUX)
        return disableClickThroughLinux(window);
    #elif defined(Q_OS_MAC)
        return disableClickThroughMac(window);
    #else
        return true;
    #endif
}

#if defined(Q_OS_WIN)

bool ClickThroughWindow::enableClickThroughWindows(QWidget* window) {
    HWND hwnd = (HWND)window->winId();
    if (!hwnd) return false;

    // 先设置 WS_EX_TRANSPARENT，不设置 WS_EX_LAYERED
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TRANSPARENT;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    // 更新窗口
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    return true;
}

bool ClickThroughWindow::disableClickThroughWindows(QWidget* window) {
    HWND hwnd = (HWND)window->winId();
    if (!hwnd) return false;

    // Remove transparent and layered styles
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle &= ~(WS_EX_TRANSPARENT | WS_EX_LAYERED);
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
    return true;
}

#elif defined(Q_OS_LINUX)
bool ClickThroughWindow::enableClickThroughLinux(QWidget* window) {
    Display* display = QX11Info::display();
    if (!display) return false;

    Window win = window->winId();

    // Create empty region for input shape
    Region region = XCreateRegion();
    XShapeCombineRegion(display, win, ShapeInput, 0, 0, region, ShapeSet);
    XDestroyRegion(region);

    return true;
}

bool ClickThroughWindow::disableClickThroughLinux(QWidget* window) {
    Display* display = QX11Info::display();
    if (!display) return false;

    Window win = window->winId();

    // Reset input shape to window bounds
    XRectangle rect = {0, 0, (unsigned short)window->width(), (unsigned short)window->height()};
    Region region = XCreateRegion();
    XUnionRectWithRegion(&rect, region, region);
    XShapeCombineRegion(display, win, ShapeInput, 0, 0, region, ShapeSet);
    XDestroyRegion(region);

    return true;
}

#elif defined(Q_OS_MAC)
bool ClickThroughWindow::enableClickThroughMac(QWidget* window) {
    NSView* view = (__bridge NSView*)reinterpret_cast<void*>(window->winId());
    if (!view) return false;

    NSWindow* nswindow = [view window];
    if (!nswindow) return false;

    // Enable click-through
    [nswindow setIgnoresMouseEvents:YES];

    return true;
}

bool ClickThroughWindow::disableClickThroughMac(QWidget* window) {
    NSView* view = (__bridge NSView*)reinterpret_cast<void*>(window->winId());
    if (!view) return false;

    NSWindow* nswindow = [view window];
    if (!nswindow) return false;

    // Disable click-through
    [nswindow setIgnoresMouseEvents:NO];

    return true;
}
#endif