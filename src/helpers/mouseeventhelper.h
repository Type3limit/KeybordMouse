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
    // 鼠标按键枚举
    enum class Button {
        Left,
        Right,
        Middle,
        Back,    // 返回键
        Forward, // 前进键
    };

    // 点击类型
    enum class ClickType {
        Single,
        Double,
        Triple
    };

private:
#ifdef _WIN32
    static DWORD getButtonDownFlag(Button button) {
        switch (button) {
            case Button::Left:    return MOUSEEVENTF_LEFTDOWN;
            case Button::Right:   return MOUSEEVENTF_RIGHTDOWN;
            case Button::Middle:  return MOUSEEVENTF_MIDDLEDOWN;
            case Button::Back:    return MOUSEEVENTF_XDOWN | (XBUTTON1 << 16);
            case Button::Forward: return MOUSEEVENTF_XDOWN | (XBUTTON2 << 16);
            default:             return MOUSEEVENTF_LEFTDOWN;
        }
    }

    static DWORD getButtonUpFlag(Button button) {
        switch (button) {
            case Button::Left:    return MOUSEEVENTF_LEFTUP;
            case Button::Right:   return MOUSEEVENTF_RIGHTUP;
            case Button::Middle:  return MOUSEEVENTF_MIDDLEUP;
            case Button::Back:    return MOUSEEVENTF_XUP | (XBUTTON1 << 16);
            case Button::Forward: return MOUSEEVENTF_XUP | (XBUTTON2 << 16);
            default:             return MOUSEEVENTF_LEFTUP;
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
            default:             return 1;
        }
    }
#elif defined(__APPLE__)
    static CGMouseButton getButtonNumber(Button button) {
        switch (button) {
            case Button::Left:    return kCGMouseButtonLeft;
            case Button::Right:   return kCGMouseButtonRight;
            case Button::Middle:  return kCGMouseButtonCenter;
            // macOS不直接支持Back/Forward按钮，需要特殊处理
            default:             return kCGMouseButtonLeft;
        }
    }

    static CGEventType getButtonDownEvent(Button button) {
        switch (button) {
            case Button::Left:    return kCGEventLeftMouseDown;
            case Button::Right:   return kCGEventRightMouseDown;
            case Button::Middle:  return kCGEventOtherMouseDown;
            default:             return kCGEventLeftMouseDown;
        }
    }

    static CGEventType getButtonUpEvent(Button button) {
        switch (button) {
            case Button::Left:    return kCGEventLeftMouseUp;
            case Button::Right:   return kCGEventRightMouseUp;
            case Button::Middle:  return kCGEventOtherMouseUp;
            default:             return kCGEventLeftMouseUp;
        }
    }
#endif

public:
    // 移动鼠标到指定位置
    static bool moveTo(int x, int y) {
#ifdef _WIN32
        return SetCursorPos(x, y);

#elif defined(__linux__)
        Display* display = XOpenDisplay(NULL);
        if (!display) return false;
        XTestFakeMotionEvent(display, -1, x, y, 0);
        XFlush(display);
        XCloseDisplay(display);
        return true;

#elif defined(__APPLE__)
        CGPoint point = CGPointMake(x, y);
        CGEventRef event = CGEventCreateMouseEvent(
            NULL, kCGEventMouseMoved, point, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, event);
        CFRelease(event);
        return true;
#endif
    }

    // 执行点击操作
    static bool click(Button button, ClickType clickType = ClickType::Single) {
        int clickCount = (clickType == ClickType::Single) ? 1 :
                        (clickType == ClickType::Double) ? 2 : 3;

        for (int i = 0; i < clickCount; i++) {
            if (!buttonDown(button) || !buttonUp(button)) {
                return false;
            }

            if (i < clickCount - 1) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
        return true;
    }

    // 按下鼠标按键
    static bool buttonDown(Button button) {
#ifdef _WIN32
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x,pos.y);
        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTDOWN, pos.x, pos.y, 0, 0);
        return true;
        // INPUT input = {};
        // input.type = INPUT_MOUSE;
        // input.mi.dwFlags = getButtonDownFlag(button);
        // return SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__linux__)
        Display* display = XOpenDisplay(NULL);
        if (!display) return false;
        XTestFakeButtonEvent(display, getButtonNumber(button), True, 0);
        XFlush(display);
        XCloseDisplay(display);
        return true;

#elif defined(__APPLE__)
        CGPoint currentPos;
        CGEventRef event = CGEventCreate(NULL);
        currentPos = CGEventGetLocation(event);
        CFRelease(event);

        CGEventRef mouseEvent = CGEventCreateMouseEvent(
            NULL, getButtonDownEvent(button), currentPos, getButtonNumber(button));
        CGEventPost(kCGHIDEventTap, mouseEvent);
        CFRelease(mouseEvent);
        return true;
#endif
    }

    // 释放鼠标按键
    static bool buttonUp(Button button) {
#ifdef _WIN32
        POINT pos;
        GetCursorPos(&pos);
        SetCursorPos(pos.x,pos.y);
        mouse_event(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_LEFTUP,pos.x, pos.y, 0, 0);
        return true;
        // INPUT input = {};
        // input.type = INPUT_MOUSE;
        // input.mi.dwFlags = getButtonUpFlag(button);
        //return SendInput(1, &input, sizeof(INPUT)) == 1;

#elif defined(__linux__)
        Display* display = XOpenDisplay(NULL);
        if (!display) return false;
        XTestFakeButtonEvent(display, getButtonNumber(button), False, 0);
        XFlush(display);
        XCloseDisplay(display);
        return true;

#elif defined(__APPLE__)
        CGPoint currentPos;
        CGEventRef event = CGEventCreate(NULL);
        currentPos = CGEventGetLocation(event);
        CFRelease(event);

        CGEventRef mouseEvent = CGEventCreateMouseEvent(
            NULL, getButtonUpEvent(button), currentPos, getButtonNumber(button));
        CGEventPost(kCGHIDEventTap, mouseEvent);
        CFRelease(mouseEvent);
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

        return buttonUp(button);
    }
};

#endif //MOUSEEVENTHELPER_H
