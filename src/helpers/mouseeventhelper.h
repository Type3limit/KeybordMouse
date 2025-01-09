//
// Created by 58226 on 2025/1/3.
//

#ifndef MOUSEEVENTHELPER_H
#define MOUSEEVENTHELPER_H

#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#elif defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <chrono>
#include <thread>

class MouseEventHelper {
public:
    enum class Button {
        Left,
        Right,
        Middle,
        Back,
        Forward,
    };

    enum class ClickType {
        Single,
        Double,
        Triple,
    };

private:
#ifdef _WIN32
    static DWORD getButtonFlag(Button button, bool down) {
        switch (button) {
            case Button::Left:    return down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
            case Button::Right:   return down ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP;
            case Button::Middle:  return down ? MOUSEEVENTF_MIDDLEDOWN : MOUSEEVENTF_MIDDLEUP;
            case Button::Back:    return down ? (MOUSEEVENTF_XDOWN | (XBUTTON1 << 16)) : (MOUSEEVENTF_XUP | (XBUTTON1 << 16));
            case Button::Forward: return down ? (MOUSEEVENTF_XDOWN | (XBUTTON2 << 16)) : (MOUSEEVENTF_XUP | (XBUTTON2 << 16));
            default:              return down ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP;
        }
    }
#elif defined(__linux__)
    static unsigned int getButtonNumber(Button button) {
        switch (button) {
            case Button::Left:    return 1;
            case Button::Right:   return 3;
            case Button::Middle:  return 2;
            case Button::Back:    return 8;
            case Button::Forward: return 9;
            default:              return 1;
        }
    }
#elif defined(__APPLE__)
    static CGMouseButton getButtonNumber(Button button) {
        switch (button) {
            case Button::Left:    return kCGMouseButtonLeft;
            case Button::Right:   return kCGMouseButtonRight;
            case Button::Middle:  return kCGMouseButtonCenter;
            default:              return kCGMouseButtonLeft;
        }
    }

    static CGEventType getMouseEvent(Button button, bool down) {
        switch (button) {
            case Button::Left:    return down ? kCGEventLeftMouseDown : kCGEventLeftMouseUp;
            case Button::Right:   return down ? kCGEventRightMouseDown : kCGEventRightMouseUp;
            case Button::Middle:  return down ? kCGEventOtherMouseDown : kCGEventOtherMouseUp;
            default:              return down ? kCGEventLeftMouseDown : kCGEventLeftMouseUp;
        }
    }
#endif

public:
    static bool moveTo(int x, int y) {
        qDebug() << "move mouse to[" << x << ":" << y<<"]";
#ifdef _WIN32
        return SetCursorPos(x, y);
#elif defined(__linux__)
        Display* display = XOpenDisplay(nullptr);
        if (!display) return false;
        XTestFakeMotionEvent(display, -1, x, y, 0);
        XFlush(display);
        XCloseDisplay(display);
        return true;
#elif defined(__APPLE__)
        CGPoint point = CGPointMake(x, y);
        CGEventRef event = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved, point, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        return true;
#endif
    }

    static bool click(Button button, ClickType clickType = ClickType::Single) {
        int clickCount = clickType == ClickType::Single ? 1 : (clickType == ClickType::Double ? 2 : 3);

        for (int i = 0; i < clickCount; ++i) {
            if (!buttonDown(button) || !buttonUp(button)) return false;
            if (i < clickCount - 1) std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        return true;
    }

    static bool buttonDown(Button button) {
#ifdef _WIN32
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = getButtonFlag(button, true);
        return SendInput(1, &input, sizeof(INPUT)) == 1;
#elif defined(__linux__)
        Display* display = XOpenDisplay(nullptr);
        if (!display) return false;
        XTestFakeButtonEvent(display, getButtonNumber(button), True, 0);
        XFlush(display);
        XCloseDisplay(display);
        return true;
#elif defined(__APPLE__)
        CGPoint currentPos = CGEventGetLocation(CGEventCreate(nullptr));
        CGEventRef event = CGEventCreateMouseEvent(nullptr, getMouseEvent(button, true), currentPos, getButtonNumber(button));
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        return true;
#endif
    }

    static bool buttonUp(Button button) {
#ifdef _WIN32
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = getButtonFlag(button, false);
        return SendInput(1, &input, sizeof(INPUT)) == 1;
#elif defined(__linux__)
        Display* display = XOpenDisplay(nullptr);
        if (!display) return false;
        XTestFakeButtonEvent(display, getButtonNumber(button), False, 0);
        XFlush(display);
        XCloseDisplay(display);
        return true;
#elif defined(__APPLE__)
        CGPoint currentPos = CGEventGetLocation(CGEventCreate(nullptr));
        CGEventRef event = CGEventCreateMouseEvent(nullptr, getMouseEvent(button, false), currentPos, getButtonNumber(button));
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        return true;
#endif
    }

    // 执行拖动操作
    static bool drag(Button button, int startX, int startY, int endX, int endY, int steps = 10) {
        if (!moveTo(startX, startY)) {
            return false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        if (!buttonDown(button)) {
            return false;
        }

        // 平滑移动
        for (int i = 1; i <= steps; i++) {
            int x = startX + ((endX - startX) * i / steps);
            int y = startY + ((endY - startY) * i / steps);

            if (!moveTo(x, y)) {
                buttonUp(button);
                return false;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return buttonUp(button);
    }
};



#endif //MOUSEEVENTHELPER_H
