//
// Created by 58226 on 2025/1/6.
//

#ifndef KEYPRESSHANDLER_H
#define KEYPRESSHANDLER_H


#include <QMap>
#include <QKeyEvent>
#include <QTimer>
#include <QElapsedTimer>

#include "globalsignal.h"
#include "mouseeventhelper.h"
#include "dialogs/fullscreenareawindow.h"
#include "qhotkey.h"

class KeyEventHandler : public QObject
{
    Q_OBJECT


public:
    static KeyEventHandler* instance();
    ~KeyEventHandler();
    void clearHotKeys();
    void setWindow(FullScreenAreaWindow* window);
    void registHotKeys();
    void handleKeyPress(QKeyEvent* event);
private:


    void SwitchScreen(QKeyEvent* event);
    void Drag(QKeyEvent* event);

    void CancelSelection(QKeyEvent* event);
    void LeftButtonClick(QKeyEvent* event);
    void RightButtonClick(QKeyEvent* event);

    void QuickLeftDoubleClick(QKeyEvent* event);
    void QuickLeftTripleClick(QKeyEvent* event);
    void QuickRightDoubleClick(QKeyEvent* event);
    void QuickRightTripleClick(QKeyEvent* event);
    void MoveColumnLeft(QKeyEvent* event);
    void MoveColumnRight(QKeyEvent* event);
    void MoveRowUp(QKeyEvent* event);
    void MoveRowDown(QKeyEvent* event);
    void MiddleButtonClick(QKeyEvent* event);
    void ForwardButtonClick(QKeyEvent* event);
    void BackwardButtonClick(QKeyEvent* event);

private:
    void handleCustomKey(QKeyEvent* event);
    void handleLeftClick();
    void handleRightClick();
    void ignoreEvent(QKeyEvent* event);
    void onKeyMoveMouse();
    void moveXPosition(int direction) const;
    void moveYPosition(int direction) const;

private:

    FullScreenAreaWindow* m_window;

    QMap<HotKeyTypes,QList<QHotkey*>> m_globalHotKeys;

    QMap<QKeySequence, void(KeyEventHandler::*)(QKeyEvent*)> m_keyActions;  // 按键映射表

    bool m_startDrag {false};

    QPoint m_currentMousePosition {};

    int m_leftClickCount;               // 左键点击次数
    int m_rightClickCount;              // 右键点击次数
    QElapsedTimer m_lastLeftClickTime;  // 左键上次点击时间
    QElapsedTimer m_lastRightClickTime; // 右键上次点击时间
    QTimer m_leftClickTimer;            // 左键点击定时器
    QTimer m_rightClickTimer;           // 右键点击定时器
    const int m_doubleClickThreshold = 400; // 双击的时间阈值（单位：毫秒）

private:
    static KeyEventHandler s_instance;
    KeyEventHandler();

};



#endif //KEYPRESSHANDLER_H
