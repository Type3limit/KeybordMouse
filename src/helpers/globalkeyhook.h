//
// Created by 58226 on 2025/1/9.
//

#ifndef GLOBALKEYHOOK_H
#define GLOBALKEYHOOK_H

#include <QObject>
#include <QSet>
#include <QHash>

#ifdef Q_OS_WIN
    #include <Windows.h>
#elif defined(Q_OS_LINUX)
    #include <X11/Xlib.h>
    #include <X11/XKBlib.h>
    #include <X11/extensions/XTest.h>
#elif defined(Q_OS_MAC)
    #include <Carbon/Carbon.h>
#endif

// 定义修饰键类型
enum class ModifierKey {
    LeftShift,
    RightShift,
    LeftControl,
    RightControl,
    LeftAlt,
    RightAlt,
    LeftMeta,
    RightMeta
};

struct KeyInfo {
    int key;            // Qt键码
    int nativeKey;      // 原生键码
    Qt::KeyboardModifiers modifiers;  // 修饰键状态
    QList<ModifierKey> nativeModifier;  // 原生修饰键
    bool isRepeat;      // 是否是重复按键
};

class GlobalKeyboardHook : public QObject {
    Q_OBJECT

public:
    explicit GlobalKeyboardHook(QObject *parent = nullptr);
    ~GlobalKeyboardHook();

    bool startHook();
    void stopHook();

    // 获取当前修饰键状态
    Qt::KeyboardModifiers currentModifiers() const;
    // 检查特定修饰键是否被按下
    bool isModifierPressed(ModifierKey modifier) const;

signals:
    void keyPressed(const KeyInfo& keyInfo);
    void keyReleased(const KeyInfo& keyInfo);

private:
    // 更新修饰键状态
    void updateModifierState(ModifierKey modifier, bool pressed);

    // 转换原生按键到Qt按键
    int nativeToQtKey(int nativeKey);

    // 计算当前的修饰键状态
    Qt::KeyboardModifiers calculateCurrentModifiers() const;

    QSet<int> pressedKeys;  // 当前按下的普通键集合
    QSet<ModifierKey> pressedModifiers;  // 当前按下的修饰键集合

#ifdef Q_OS_WIN
    static HHOOK keyboardHook;
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    // Windows特定的虚拟键码到修饰键的映射
    static ModifierKey mapVirtualKeyToModifier(DWORD vkCode);
#elif defined(Q_OS_LINUX)
    Display* display;
    Window root;
    bool setupX11Hook();
    void cleanupX11Hook();
    static int x11ErrorHandler(Display* display, XErrorEvent* error);
    void grabAllKeys();
    void ungrabAllKeys();

    // X11特定的键码到修饰键的映射
    ModifierKey mapX11KeyToModifier(KeyCode keycode);
#elif defined(Q_OS_MAC)
    static OSStatus macKeyboardHandler(EventHandlerCallRef nextHandler,
                                     EventRef theEvent,
                                     void* userData);
    EventHandlerRef eventHandler;

    // Mac特定的键码到修饰键的映射
    static ModifierKey mapMacKeyToModifier(UInt32 keycode);
#endif

    static GlobalKeyboardHook* instance;
    bool isHooked;
};



#endif //GLOBALKEYHOOK_H
