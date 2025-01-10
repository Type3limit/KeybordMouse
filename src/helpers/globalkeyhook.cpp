#include "globalkeyhook.h"
#include <QApplication>
#include <QKeyEvent>

GlobalKeyboardHook* GlobalKeyboardHook::instance = nullptr;

#ifdef Q_OS_WIN
HHOOK GlobalKeyboardHook::keyboardHook = NULL;
#endif

GlobalKeyboardHook::GlobalKeyboardHook(QObject *parent)
    : QObject(parent), isHooked(false) {
    instance = this;
#ifdef Q_OS_LINUX
    display = nullptr;
#elif defined(Q_OS_MAC)
    eventHandler = nullptr;
#endif
}

GlobalKeyboardHook::~GlobalKeyboardHook() {
    stopHook();
    instance = nullptr;
}

bool GlobalKeyboardHook::startHook()
{
    if (isHooked) return true;

#ifdef Q_OS_WIN
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL,
                                   keyboardProc,
                                   GetModuleHandle(NULL),
                                   0);
    isHooked = (keyboardHook != NULL);

#elif defined(Q_OS_LINUX)
    isHooked = setupX11Hook();

#elif defined(Q_OS_MAC)
    EventTypeSpec eventTypes[2];
    // 按键按下事件
    eventTypes[0].eventClass = kEventClassKeyboard;
    eventTypes[0].eventKind = kEventRawKeyDown;
    // 按键释放事件
    eventTypes[1].eventClass = kEventClassKeyboard;
    eventTypes[1].eventKind = kEventRawKeyUp;

    OSStatus status = InstallEventHandler(GetEventMonitorTarget(),
        NewEventHandlerUPP(macKeyboardHandler),
        2, eventTypes,
        this, &eventHandler);

    isHooked = (status == noErr);
#endif

    return isHooked;
}

void GlobalKeyboardHook::stopHook()
{
    if (!isHooked) return;

#ifdef Q_OS_WIN
    if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = NULL;
    }

#elif defined(Q_OS_LINUX)
    cleanupX11Hook();

#elif defined(Q_OS_MAC)
    if (eventHandler) {
        RemoveEventHandler(eventHandler);
        eventHandler = nullptr;
    }
#endif

    isHooked = false;
    pressedKeys.clear();
}

Qt::KeyboardModifiers GlobalKeyboardHook::calculateCurrentModifiers() const {
    Qt::KeyboardModifiers mods = Qt::NoModifier;

    // Shift
    if (pressedModifiers.contains(ModifierKey::LeftShift) ||
        pressedModifiers.contains(ModifierKey::RightShift)) {
        mods |= Qt::ShiftModifier;
    }

    // Control
    if (pressedModifiers.contains(ModifierKey::LeftControl) ||
        pressedModifiers.contains(ModifierKey::RightControl)) {
        mods |= Qt::ControlModifier;
    }

    // Alt
    if (pressedModifiers.contains(ModifierKey::LeftAlt) ||
        pressedModifiers.contains(ModifierKey::RightAlt)) {
        mods |= Qt::AltModifier;
    }

    // Meta (Windows key/Command key)
    if (pressedModifiers.contains(ModifierKey::LeftMeta) ||
        pressedModifiers.contains(ModifierKey::RightMeta)) {
        mods |= Qt::MetaModifier;
    }

    return mods;
}

void GlobalKeyboardHook::updateModifierState(ModifierKey modifier, bool pressed) {
    if (pressed) {
        pressedModifiers.insert(modifier);
    } else {
        pressedModifiers.remove(modifier);
    }
}


int GlobalKeyboardHook::nativeToQtKey(int nativeKeyCode)
{
#ifdef Q_OS_WIN
    // Windows 虚拟键码到 Qt 键码的映射
    switch (nativeKeyCode)
    {
    case VK_CONTROL: return Qt::Key_Control;
    case VK_SHIFT: return Qt::Key_Shift;
    case VK_MENU: return Qt::Key_Alt;
    case VK_LWIN: case VK_RWIN: return Qt::Key_Meta;
    case VK_ESCAPE: return Qt::Key_Escape;
    case VK_RETURN: return Qt::Key_Return;
    case VK_SPACE: return Qt::Key_Space;
    case VK_BACK: return Qt::Key_Backspace;
    case VK_TAB: return Qt::Key_Tab;
    case VK_CAPITAL: return Qt::Key_CapsLock;
    case VK_LEFT: return Qt::Key_Left;
    case VK_RIGHT: return Qt::Key_Right;
    case VK_UP: return Qt::Key_Up;
    case VK_DOWN: return Qt::Key_Down;
    case VK_PRIOR: return Qt::Key_PageUp;
    case VK_NEXT: return Qt::Key_PageDown;
    case VK_HOME: return Qt::Key_Home;
    case VK_END: return Qt::Key_End;
    case VK_INSERT: return Qt::Key_Insert;
    case VK_DELETE: return Qt::Key_Delete;
    case VK_F1: return Qt::Key_F1;
    case VK_F2: return Qt::Key_F2;
    case VK_F3: return Qt::Key_F3;
    case VK_F4: return Qt::Key_F4;
    case VK_F5: return Qt::Key_F5;
    case VK_F6: return Qt::Key_F6;
    case VK_F7: return Qt::Key_F7;
    case VK_F8: return Qt::Key_F8;
    case VK_F9: return Qt::Key_F9;
    case VK_F10: return Qt::Key_F10;
    case VK_F11: return Qt::Key_F11;
    case VK_F12: return Qt::Key_F12;
    case VK_OEM_MINUS: return Qt::Key_Minus;       // -
    case VK_OEM_PLUS: return Qt::Key_Equal;        // =
    case VK_OEM_4: return Qt::Key_BracketLeft;     // [
    case VK_OEM_6: return Qt::Key_BracketRight;    // ]
    case VK_OEM_1: return Qt::Key_Semicolon;       // ;
    case VK_OEM_7: return Qt::Key_Apostrophe;      // '
    case VK_OEM_COMMA: return Qt::Key_Comma;       // ,
    case VK_OEM_PERIOD: return Qt::Key_Period;     // .
    case VK_OEM_2: return Qt::Key_Slash;           // /
    case VK_OEM_5: return Qt::Key_Backslash;       //
    case VK_OEM_3: return Qt::Key_QuoteLeft;       // `
    case 'A': return Qt::Key_A;
    case 'B': return Qt::Key_B;
    case 'C': return Qt::Key_C;
    case 'D': return Qt::Key_D;
    case 'E': return Qt::Key_E;
    case 'F': return Qt::Key_F;
    case 'G': return Qt::Key_G;
    case 'H': return Qt::Key_H;
    case 'I': return Qt::Key_I;
    case 'J': return Qt::Key_J;
    case 'K': return Qt::Key_K;
    case 'L': return Qt::Key_L;
    case 'M': return Qt::Key_M;
    case 'N': return Qt::Key_N;
    case 'O': return Qt::Key_O;
    case 'P': return Qt::Key_P;
    case 'Q': return Qt::Key_Q;
    case 'R': return Qt::Key_R;
    case 'S': return Qt::Key_S;
    case 'T': return Qt::Key_T;
    case 'U': return Qt::Key_U;
    case 'V': return Qt::Key_V;
    case 'W': return Qt::Key_W;
    case 'X': return Qt::Key_X;
    case 'Y': return Qt::Key_Y;
    case 'Z': return Qt::Key_Z;
    case '0': return Qt::Key_0;
    case '1': return Qt::Key_1;
    case '2': return Qt::Key_2;
    case '3': return Qt::Key_3;
    case '4': return Qt::Key_4;
    case '5': return Qt::Key_5;
    case '6': return Qt::Key_6;
    case '7': return Qt::Key_7;
    case '8': return Qt::Key_8;
    case '9': return Qt::Key_9;
    default: return Qt::Key_unknown;
    }
#endif

#ifdef Q_OS_MAC
    // macOS 键码到 Qt 键码的映射
    switch (nativeKeyCode)
    {
    case kVK_Shift: return Qt::Key_Shift;
    case kVK_Control: return Qt::Key_Control;
    case kVK_Option: return Qt::Key_Alt;
    case kVK_Command: return Qt::Key_Meta;
    case kVK_Space: return Qt::Key_Space;
    case kVK_Return: return Qt::Key_Return;
    case kVK_Tab: return Qt::Key_Tab;
    case kVK_Escape: return Qt::Key_Escape;
    case kVK_LeftArrow: return Qt::Key_Left;
    case kVK_RightArrow: return Qt::Key_Right;
    case kVK_UpArrow: return Qt::Key_Up;
    case kVK_DownArrow: return Qt::Key_Down;
    case kVK_PageUp: return Qt::Key_PageUp;
    case kVK_PageDown: return Qt::Key_PageDown;
    case kVK_Home: return Qt::Key_Home;
    case kVK_End: return Qt::Key_End;
    case kVK_ForwardDelete: return Qt::Key_Delete;
    case kVK_ANSI_Minus: return Qt::Key_Minus;     // -
    case kVK_ANSI_Equal: return Qt::Key_Equal;     // =
    case kVK_ANSI_LeftBracket: return Qt::Key_BracketLeft;  // [
    case kVK_ANSI_RightBracket: return Qt::Key_BracketRight; // ]
    case kVK_ANSI_Semicolon: return Qt::Key_Semicolon;      // ;
    case kVK_ANSI_Quote: return Qt::Key_Apostrophe;         // '
    case kVK_ANSI_Comma: return Qt::Key_Comma;              // ,
    case kVK_ANSI_Period: return Qt::Key_Period;            // .
    case kVK_ANSI_Slash: return Qt::Key_Slash;              // /
    case kVK_ANSI_Backslash: return Qt::Key_Backslash;      //
    case kVK_ANSI_Grave: return Qt::Key_QuoteLeft;          // `
    case kVK_F1: return Qt::Key_F1;
    case kVK_F2: return Qt::Key_F2;
    case kVK_F3: return Qt::Key_F3;
    case kVK_F4: return Qt::Key_F4;
    case kVK_F5: return Qt::Key_F5;
    case kVK_F6: return Qt::Key_F6;
    case kVK_F7: return Qt::Key_F7;
    case kVK_F8: return Qt::Key_F8;
    case kVK_F9: return Qt::Key_F9;
    case kVK_F10: return Qt::Key_F10;
    case kVK_F11: return Qt::Key_F11;
    case kVK_F12: return Qt::Key_F12;
    case kVK_ANSI_A: return Qt::Key_A;
    case kVK_ANSI_B: return Qt::Key_B;
    case kVK_ANSI_C: return Qt::Key_C;
    case kVK_ANSI_D: return Qt::Key_D;
    case kVK_ANSI_E: return Qt::Key_E;
    case kVK_ANSI_F: return Qt::Key_F;
    case kVK_ANSI_G: return Qt::Key_G;
    case kVK_ANSI_H: return Qt::Key_H;
    case kVK_ANSI_I: return Qt::Key_I;
    case kVK_ANSI_J: return Qt::Key_J;
    case kVK_ANSI_K: return Qt::Key_K;
    case kVK_ANSI_L: return Qt::Key_L;
    case kVK_ANSI_M: return Qt::Key_M;
    case kVK_ANSI_N: return Qt::Key_N;
    case kVK_ANSI_O: return Qt::Key_O;
    case kVK_ANSI_P: return Qt::Key_P;
    case kVK_ANSI_Q: return Qt::Key_Q;
    case kVK_ANSI_R: return Qt::Key_R;
    case kVK_ANSI_S: return Qt::Key_S;
    case kVK_ANSI_T: return Qt::Key_T;
    case kVK_ANSI_U: return Qt::Key_U;
    case kVK_ANSI_V: return Qt::Key_V;
    case kVK_ANSI_W: return Qt::Key_W;
    case kVK_ANSI_X: return Qt::Key_X;
    case kVK_ANSI_Y: return Qt::Key_Y;
    case kVK_ANSI_Z: return Qt::Key_Z;
    case kVK_ANSI_0: return Qt::Key_0;
    case kVK_ANSI_1: return Qt::Key_1;
    case kVK_ANSI_2: return Qt::Key_2;
    case kVK_ANSI_3: return Qt::Key_3;
    case kVK_ANSI_4: return Qt::Key_4;
    case kVK_ANSI_5: return Qt::Key_5;
    case kVK_ANSI_6: return Qt::Key_6;
    case kVK_ANSI_7: return Qt::Key_7;
    case kVK_ANSI_8: return Qt::Key_8;
    case kVK_ANSI_9: return Qt::Key_9;
    default: return Qt::Key_unknown;
    }
#endif

#ifdef Q_OS_LINUX
    // Linux X11 键码到 Qt 键码的映射
    KeySym keySym = XKeycodeToKeysym(display, nativeKeyCode, 0);
    switch (keySym)
    {
    case XK_Shift_L: case XK_Shift_R: return Qt::Key_Shift;
    case XK_Control_L: case XK_Control_R: return Qt::Key_Control;
    case XK_Alt_L: case XK_Alt_R: return Qt::Key_Alt;
    case XK_Super_L: case XK_Super_R: return Qt::Key_Meta;
    case XK_space: return Qt::Key_Space;
    case XK_Return: return Qt::Key_Return;
    case XK_Escape: return Qt::Key_Escape;
    case XK_BackSpace: return Qt::Key_Backspace;
    case XK_Tab: return Qt::Key_Tab;
    case XK_Left: return Qt::Key_Left;
    case XK_Right: return Qt::Key_Right;
    case XK_Up: return Qt::Key_Up;
    case XK_Down: return Qt::Key_Down;
    case XK_Page_Up: return Qt::Key_PageUp;
    case XK_Page_Down: return Qt::Key_PageDown;
    case XK_Home: return Qt::Key_Home;
    case XK_End: return Qt::Key_End;
    case XK_Insert: return Qt::Key_Insert;
    case XK_Delete: return Qt::Key_Delete;
    case XK_minus: return Qt::Key_Minus;           // -
    case XK_equal: return Qt::Key_Equal;           // =
    case XK_bracketleft: return Qt::Key_BracketLeft;  // [
    case XK_bracketright: return Qt::Key_BracketRight; // ]
    case XK_semicolon: return Qt::Key_Semicolon;      // ;
    case XK_apostrophe: return Qt::Key_Apostrophe;    // '
    case XK_comma: return Qt::Key_Comma;              // ,
    case XK_period: return Qt::Key_Period;            // .
    case XK_slash: return Qt::Key_Slash;              // /
    case XK_backslash: return Qt::Key_Backslash;      //
    case XK_grave: return Qt::Key_QuoteLeft;          // `
    case XK_F1: return Qt::Key_F1;
    case XK_F2: return Qt::Key_F2;
    case XK_F3: return Qt::Key_F3;
    case XK_F4: return Qt::Key_F4;
    case XK_F5: return Qt::Key_F5;
    case XK_F6: return Qt::Key_F6;
    case XK_F7: return Qt::Key_F7;
    case XK_F8: return Qt::Key_F8;
    case XK_F9: return Qt::Key_F9;
    case XK_F10: return Qt::Key_F10;
    case XK_F11: return Qt::Key_F11;
    case XK_F12: return Qt::Key_F12;
    case XK_A: case XK_a: return Qt::Key_A;
    case XK_B: case XK_b: return Qt::Key_B;
    case XK_C: case XK_c: return Qt::Key_C;
    case XK_D: case XK_d: return Qt::Key_D;
    case XK_E: case XK_e: return Qt::Key_E;
    case XK_F: case XK_f: return Qt::Key_F;
    case XK_G: case XK_g: return Qt::Key_G;
    case XK_H: case XK_h: return Qt::Key_H;
    case XK_I: case XK_i: return Qt::Key_I;
    case XK_J: case XK_j: return Qt::Key_J;
    case XK_K: case XK_k: return Qt::Key_K;
    case XK_L: case XK_l: return Qt::Key_L;
    case XK_M: case XK_m: return Qt::Key_M;
    case XK_N: case XK_n: return Qt::Key_N;
    case XK_O: case XK_o: return Qt::Key_O;
    case XK_P: case XK_p: return Qt::Key_P;
    case XK_Q: case XK_q: return Qt::Key_Q;
    case XK_R: case XK_r: return Qt::Key_R;
    case XK_S: case XK_s: return Qt::Key_S;
    case XK_T: case XK_t: return Qt::Key_T;
    case XK_U: case XK_u: return Qt::Key_U;
    case XK_V: case XK_v: return Qt::Key_V;
    case XK_W: case XK_w: return Qt::Key_W;
    case XK_X: case XK_x: return Qt::Key_X;
    case XK_Y: case XK_y: return Qt::Key_Y;
    case XK_Z: case XK_z: return Qt::Key_Z;
    case XK_0: return Qt::Key_0;
    case XK_1: return Qt::Key_1;
    case XK_2: return Qt::Key_2;
    case XK_3: return Qt::Key_3;
    case XK_4: return Qt::Key_4;
    case XK_5: return Qt::Key_5;
    case XK_6: return Qt::Key_6;
    case XK_7: return Qt::Key_7;
    case XK_8: return Qt::Key_8;
    case XK_9: return Qt::Key_9;
    default: return Qt::Key_unknown;
    }
#endif
}

bool GlobalKeyboardHook::isModifierPressed(ModifierKey modifier) const {
    return pressedModifiers.contains(modifier);
}

Qt::KeyboardModifiers GlobalKeyboardHook::currentModifiers() const {
    return calculateCurrentModifiers();
}


#ifdef Q_OS_WIN
ModifierKey GlobalKeyboardHook::mapVirtualKeyToModifier(DWORD vkCode) {
    switch (vkCode) {
        case VK_LSHIFT: return ModifierKey::LeftShift;
        case VK_RSHIFT: return ModifierKey::RightShift;
        case VK_LCONTROL: return ModifierKey::LeftControl;
        case VK_RCONTROL: return ModifierKey::RightControl;
        case VK_LMENU: return ModifierKey::LeftAlt;
        case VK_RMENU: return ModifierKey::RightAlt;
        case VK_LWIN: return ModifierKey::LeftMeta;
        case VK_RWIN: return ModifierKey::RightMeta;
        default: throw std::invalid_argument("Not a modifier key");
    }
}

LRESULT CALLBACK GlobalKeyboardHook::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && instance) {
        KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;
        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

        try {
            // 尝试将按键识别为修饰键
            ModifierKey modifier = mapVirtualKeyToModifier(kbStruct->vkCode);
            instance->updateModifierState(modifier, isKeyDown);
        } catch (const std::invalid_argument&) {
            // 不是修饰键，作为普通键处理
            if (isKeyDown) {
                instance->pressedKeys.insert(kbStruct->vkCode);
            } else {
                instance->pressedKeys.remove(kbStruct->vkCode);
            }
        }

        // 发送按键事件
        KeyInfo keyInfo;
        keyInfo.nativeKey = kbStruct->vkCode;
        keyInfo.key = instance->nativeToQtKey(kbStruct->vkCode);
        keyInfo.nativeModifier = instance->pressedModifiers.values();
        keyInfo.modifiers = instance->calculateCurrentModifiers();
        keyInfo.isRepeat = (kbStruct->flags & LLKHF_EXTENDED) != 0;

        if (isKeyDown) {
            emit instance->keyPressed(keyInfo);
        } else {
            emit instance->keyReleased(keyInfo);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

#elif defined(Q_OS_LINUX)
ModifierKey GlobalKeyboardHook::mapX11KeyToModifier(KeyCode keycode) {
    static XkbDescPtr xkb = XkbGetMap(display, XkbAllComponentsMask, XkbUseCoreKbd);
    KeySym keysym = XkbKeycodeToKeysym(display, keycode, 0, 0);

    switch (keysym) {
        case XK_Shift_L: return ModifierKey::LeftShift;
        case XK_Shift_R: return ModifierKey::RightShift;
        case XK_Control_L: return ModifierKey::LeftControl;
        case XK_Control_R: return ModifierKey::RightControl;
        case XK_Alt_L: return ModifierKey::LeftAlt;
        case XK_Alt_R: return ModifierKey::RightAlt;
        case XK_Super_L: return ModifierKey::LeftMeta;
        case XK_Super_R: return ModifierKey::RightMeta;
        default: throw std::invalid_argument("Not a modifier key");
    }
}

bool GlobalKeyboardHook::setupX11Hook() {
    // ... 之前的X11设置代码 ...

    QThread* eventThread = QThread::create([this]() {
        XEvent event;
        while (isHooked) {
            if (XPending(display)) {
                XNextEvent(display, &event);
                bool isKeyPress = (event.type == KeyPress);

                if (event.type == KeyPress || event.type == KeyRelease) {
                    try {
                        // 尝试将按键识别为修饰键
                        ModifierKey modifier = mapX11KeyToModifier(event.xkey.keycode);
                        updateModifierState(modifier, isKeyPress);
                    } catch (const std::invalid_argument&) {
                        // 不是修饰键，作为普通键处理
                        if (isKeyPress) {
                            pressedKeys.insert(event.xkey.keycode);
                        } else {
                            pressedKeys.remove(event.xkey.keycode);
                        }
                    }

                    // 发送按键事件
                    KeyInfo keyInfo;
                    keyInfo.nativeKey = event.xkey.keycode;
                    keyInfo.key = nativeToQtKey(event.xkey.keycode);
                    keyInfo.modifiers = calculateCurrentModifiers();
                    keyInfo.isRepeat = false;  // 需要额外逻辑来检测重复键

                    if (isKeyPress) {
                        emit keyPressed(keyInfo);
                    } else {
                        emit keyReleased(keyInfo);
                    }
                }
            }
            QThread::msleep(10);
        }
    });
    eventThread->start();

    return true;
}

#elif defined(Q_OS_MAC)
ModifierKey GlobalKeyboardHook::mapMacKeyToModifier(UInt32 keycode) {
    switch (keycode) {
        case kVK_Shift: return ModifierKey::LeftShift;
        case kVK_RightShift: return ModifierKey::RightShift;
        case kVK_Control: return ModifierKey::LeftControl;
        case kVK_RightControl: return ModifierKey::RightControl;
        case kVK_Option: return ModifierKey::LeftAlt;
        case kVK_RightOption: return ModifierKey::RightAlt;
        case kVK_Command: return ModifierKey::LeftMeta;
        case kVK_RightCommand: return ModifierKey::RightMeta;
        default: throw std::invalid_argument("Not a modifier key");
    }
}

OSStatus GlobalKeyboardHook::macKeyboardHandler(EventHandlerCallRef nextHandler,
                                              EventRef theEvent,
                                              void* userData) {
    GlobalKeyboardHook* hook = static_cast<GlobalKeyboardHook*>(userData);
    if (!hook) return noErr;

    UInt32 keycode;
    GetEventParameter(theEvent, kEventParamKeyCode, typeUInt32, nullptr,
                     sizeof(UInt32), nullptr, &keycode);

    UInt32 eventKind = GetEventKind(theEvent);
    bool isKeyDown = (eventKind == kEventRawKeyDown);

    try {
        // 尝试将按键识别为修饰键
        ModifierKey modifier = mapMacKeyToModifier(keycode);
        hook->updateModifierState(modifier, isKeyDown);
    } catch (const std::invalid_argument&) {
        // 不是修饰键，作为普通键处理
        if (isKeyDown) {
            hook->pressedKeys.insert(keycode);
        } else {
            hook->pressedKeys.remove(keycode);
        }
    }

    // 发送按键事件
    KeyInfo keyInfo;
    keyInfo.nativeKey = keycode;
    keyInfo.key = hook->nativeToQtKey(keycode);
    keyInfo.modifiers = hook->calculateCurrentModifiers();

    Boolean isRepeat = false;
    GetEventParameter(theEvent, kEventParamKeyMacCharCodes, typeBoolean, nullptr,
                     sizeof(Boolean), nullptr, &isRepeat);
    keyInfo.isRepeat = isRepeat;

    if (isKeyDown) {
        emit hook->keyPressed(keyInfo);
    } else {
        emit hook->keyReleased(keyInfo);
    }

    return CallNextEventHandler(nextHandler, theEvent);
}
#endif