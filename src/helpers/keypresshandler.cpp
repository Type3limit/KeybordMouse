//
// Created by 58226 on 2025/1/7.
//
#include "keypresshandler.h"


#define REGIST_HOTKEY(HOTKEY_NAME)     \
    auto HOTKEY_NAME##HotKeys = Config::instance()->getHotkeySequence(HOTKEY_NAME##HotKey);\
    for (auto &key : HOTKEY_NAME##HotKeys)\
    {\
        m_keyActions[key] = &KeyEventHandler::HOTKEY_NAME;\
    }

#define REGIST_GLOBAL_HOTKEY(TYPE,DONEXT)\
    auto TYPE##s = Config::instance()->getHotkeySequence(ConfigType::TYPE);\
    for (auto& key : TYPE##s)\
    {\
        auto curHotKey = new QHotkey(key,true,nullptr);\
        m_globalHotKeys[TYPE].push_back(curHotKey);\
        connect(curHotKey,&QHotkey::activated,this,[&]()\
        {\
        DONEXT;\
        });\
    }

KeyEventHandler KeyEventHandler::s_instance;

KeyEventHandler::KeyEventHandler()
{
    // 定时器，每次点击后启动定时器，延迟处理
    m_leftClickTimer.setSingleShot(true);
    m_leftClickTimer.setInterval(m_doubleClickThreshold);
    connect(&m_leftClickTimer, &QTimer::timeout, this, [&]()
    {
        qDebug()<<"left click timer time out:"<<m_leftClickCount;
        KeyEventHandler::handleLeftClick();
    });

    m_rightClickTimer.setSingleShot(true);
    m_rightClickTimer.setInterval(m_doubleClickThreshold);
    connect(&m_rightClickTimer, &QTimer::timeout, this, [&]()
    {
        qDebug()<<"right click timer time out:"<<m_rightClickCount;
        KeyEventHandler::handleRightClick();
    });
}

KeyEventHandler* KeyEventHandler::instance()
{
    return &s_instance;
}

void KeyEventHandler::clearHotKeys()
{
    auto res = m_globalHotKeys.values();
    for (const auto& itr : res)
    {
        for (auto& key : itr)
        {
            delete key;
        }
    }
    m_globalHotKeys.clear();
    m_keyActions.clear();
}

KeyEventHandler::~KeyEventHandler()
{

}

void KeyEventHandler::setWindow(FullScreenAreaWindow* window)
{
    m_window = window;
}

void KeyEventHandler::handleKeyPress(QKeyEvent* event)
{
    auto curKeySequence = QKeySequence(event->key() | event->modifiers());
    if (m_keyActions.contains(curKeySequence)) {
        (this->*m_keyActions[curKeySequence])(event);
    } else {
        handleCustomKey(event);
    }
}

void KeyEventHandler::registHotKeys()
{
    //clear all
    clearHotKeys();
    //global hotkey
    REGIST_GLOBAL_HOTKEY(ShowFullScreenHotKey,GlobalSignal::instance()->requestOpenFullScreenWindow())
    REGIST_GLOBAL_HOTKEY(CloseWindowHotKey,GlobalSignal::instance()->requestCloseFullScreenWindow())
    REGIST_GLOBAL_HOTKEY(OpenSettingWindowHotKey,GlobalSignal::instance()->requestOpenSettingWindow())
    REGIST_GLOBAL_HOTKEY(CloseSettingWindowHotKey,GlobalSignal::instance()->requestCloseSettingWindow())
    //customized hotkeys
    REGIST_HOTKEY(CancelSelection)
    REGIST_HOTKEY(SwitchScreen)
    REGIST_HOTKEY(LeftButtonClick)
    REGIST_HOTKEY(RightButtonClick)
    REGIST_HOTKEY(Drag)
    REGIST_HOTKEY(QuickLeftDoubleClick)
    REGIST_HOTKEY(QuickLeftTripleClick)
    REGIST_HOTKEY(QuickRightDoubleClick)
    REGIST_HOTKEY(QuickRightTripleClick)
    REGIST_HOTKEY(MoveColumnLeft)
    REGIST_HOTKEY(MoveColumnRight)
    REGIST_HOTKEY(MoveRowUp)
    REGIST_HOTKEY(MoveRowDown)
    REGIST_HOTKEY(MiddleButtonClick)
    REGIST_HOTKEY(ForwardButtonClick)
    REGIST_HOTKEY(BackwardButtonClick)
}


void KeyEventHandler::CancelSelection(QKeyEvent* event)
{
    if (m_window->m_thirdPos>=0)
    {
        m_window->m_thirdPos = -1;
    }
    else if (m_window->m_secondPos>=0)
    {
        m_window->m_secondPos = -1;
    }
    else if (m_window->m_firstPos>=0)
    {
        m_window-> m_firstPos = -1;
    }
    event->ignore();
}

void KeyEventHandler::ignoreEvent(QKeyEvent* event)
{
    event->ignore();
}

void KeyEventHandler::SwitchScreen(QKeyEvent* event)
{

    auto screens = QGuiApplication::screens();
    auto curIndex = screens.indexOf(m_window->currentScreen());
    curIndex += 1 ;

    if (curIndex >= screens.count()) curIndex = 0;
    if (curIndex < 0) curIndex = screens.count() - 1;

    m_window->setCurrentScreen(screens[curIndex]);
    m_window->setGeometry(m_window->currentScreen()->geometry());
    event->accept();
}

void KeyEventHandler::Drag(QKeyEvent* event)
{
    if (m_startDrag)
    {
        m_startDrag = false;
        m_window->m_dragEndPos = m_currentMousePosition;
        qDebug()<<"drag end at:"<<m_window->m_dragEndPos;
        m_window->update();
        auto curScreenPix = m_window->currentScreen()->devicePixelRatio();
        MouseEventHelper::drag(MouseEventHelper::Button::Left,
                               m_window->m_dragStartPos.x(),
                               m_window->m_dragStartPos.y()*curScreenPix,
                               m_window->m_dragEndPos.x() / curScreenPix,
                               m_window->m_dragEndPos.y()*curScreenPix);
        m_window->m_dragStartPos = QPoint(INT_MIN,INT_MIN);
        m_window->m_dragEndPos = QPoint(INT_MIN,INT_MIN);
        m_window->close();

    }
    else
    {
        m_startDrag = true;
        m_window->m_dragStartPos = m_currentMousePosition;
        qDebug()<<"drag start at:"<<m_window->m_dragStartPos;
        m_window->update();
    }
}

void KeyEventHandler::LeftButtonClick(QKeyEvent* event)
{

        qint64 currentTime = m_lastLeftClickTime.elapsed();

        // 判断是否在双击阈值时间内
        if (currentTime <= m_doubleClickThreshold) {
            // 在双击时间内
            m_leftClickCount++;
        } else {
            // 不是双击，重置点击次数
            m_leftClickCount = 1;
        }

        // 更新最后一次点击时间
        m_lastLeftClickTime.restart();

        // 启动定时器，判断是否是单击、双击、三击
        m_leftClickTimer.start();

}

void KeyEventHandler::RightButtonClick(QKeyEvent* event)
{
    qint64 currentTime = m_lastRightClickTime.elapsed();

    // 判断是否在双击阈值时间内
    if (currentTime <= m_doubleClickThreshold) {
        // 在双击时间内
        m_rightClickCount++;
    } else {
        // 不是双击，重置点击次数
        m_rightClickCount = 1;
    }

    // 更新最后一次点击时间
    m_lastRightClickTime.restart();

    // 启动定时器，判断是否是单击、双击、三击
    m_rightClickTimer.start();
}

void KeyEventHandler::handleLeftClick()
{
    if (m_leftClickCount == 1) {
        qDebug()<<"send left click";
        MouseEventHelper::click(MouseEventHelper::Button::Left, MouseEventHelper::ClickType::Single);
    } else if (m_leftClickCount == 2) {
        qDebug()<<"send left double click";
        MouseEventHelper::click(MouseEventHelper::Button::Left, MouseEventHelper::ClickType::Double);
    } else if (m_leftClickCount >= 3) {
        qDebug()<<"send left triple click";
        MouseEventHelper::click(MouseEventHelper::Button::Left, MouseEventHelper::ClickType::Triple);
    }

    // 重置点击次数
    m_leftClickCount = 0;
    m_window->close();
}

void KeyEventHandler::handleRightClick()
{
    if (m_rightClickCount == 1) {
        qDebug()<<"send left click";
        MouseEventHelper::click(MouseEventHelper::Button::Right, MouseEventHelper::ClickType::Single);
    } else if (m_rightClickCount == 2) {
        qDebug()<<"send left double click";
        MouseEventHelper::click(MouseEventHelper::Button::Right, MouseEventHelper::ClickType::Double);
    } else if (m_rightClickCount >= 3) {
        qDebug()<<"send left triple click";
        MouseEventHelper::click(MouseEventHelper::Button::Right, MouseEventHelper::ClickType::Triple);
    }

    // 重置点击次数
    m_rightClickCount = 0;
}

void KeyEventHandler::QuickLeftDoubleClick(QKeyEvent* event)
{
    qDebug()<<"send quick left double click";
    MouseEventHelper::click(MouseEventHelper::Button::Left, MouseEventHelper::ClickType::Double);
}

void KeyEventHandler::QuickLeftTripleClick(QKeyEvent* event)
{
    qDebug()<<"send quick left triple click";
    MouseEventHelper::click(MouseEventHelper::Button::Left, MouseEventHelper::ClickType::Triple);
}

void KeyEventHandler::QuickRightDoubleClick(QKeyEvent* event)
{
    qDebug()<<"send quick right double click";
    MouseEventHelper::click(MouseEventHelper::Button::Right, MouseEventHelper::ClickType::Double);
}

void KeyEventHandler::QuickRightTripleClick(QKeyEvent* event)
{
    qDebug()<<"send quick right double click";
    MouseEventHelper::click(MouseEventHelper::Button::Right, MouseEventHelper::ClickType::Triple);
}

void KeyEventHandler::MoveColumnLeft(QKeyEvent* event)
{
    qDebug()<<"send move column left";
    moveXPosition(-1);
}

void KeyEventHandler::MoveColumnRight(QKeyEvent* event)
{
    qDebug()<<"send move column right";
    moveXPosition(1);
}

void KeyEventHandler::MoveRowUp(QKeyEvent* event)
{
    qDebug()<<"send move row up";
    moveYPosition(-1);
}

void KeyEventHandler::MoveRowDown(QKeyEvent* event)
{
    qDebug()<<"send move row down";
    moveYPosition(1);
}

void KeyEventHandler::MiddleButtonClick(QKeyEvent* event)
{
    qDebug()<<"send middle button click";
    MouseEventHelper::click(MouseEventHelper::Button::Middle, MouseEventHelper::ClickType::Single);
}

void KeyEventHandler::ForwardButtonClick(QKeyEvent* event)
{
    qDebug()<<"send forward button click";
    MouseEventHelper::click(MouseEventHelper::Button::Forward, MouseEventHelper::ClickType::Single);
}

void KeyEventHandler::BackwardButtonClick(QKeyEvent* event)
{
    qDebug()<<"send backward button click";
    MouseEventHelper::click(MouseEventHelper::Button::Back, MouseEventHelper::ClickType::Single);
}

void KeyEventHandler::moveXPosition(int direction) const
{
    auto optionMode = Config::instance()->getOptionMode();
    auto subCellMode = Config::instance()->getSubCellOptionMode();
    if (m_window->m_thirdPos < 0)
    {
        m_window->m_firstPos += direction;
        if (m_window->m_firstPos < 0)
        {
            m_window->m_firstPos = ScreenOptionModeStr[optionMode].count() - 1;
        }
        else if (m_window->m_firstPos >= ScreenOptionModeStr[optionMode].count())
        {
            m_window->m_firstPos = 0;
        }
    }
    else
    {
        m_window->m_thirdPos += direction;
        if (m_window->m_thirdPos < 0)
        {
            m_window->m_thirdPos = ScreenOptionModeStr[subCellMode].count() - 1;
        }
        else if (m_window->m_thirdPos >= ScreenOptionModeStr[subCellMode].count())
        {
            m_window->m_thirdPos = 0;
        }
    }
}

void KeyEventHandler::moveYPosition(int direction) const
{
    auto optionMode = Config::instance()->getOptionMode();
    auto subCellMode = Config::instance()->getSubCellOptionMode();
    if (m_window->m_thirdPos < 0)
    {
        m_window->m_secondPos += direction;
        if (m_window->m_secondPos < 0)
        {
            m_window->m_secondPos = ScreenOptionModeStr[optionMode].count() - 1;
        }
        else if (m_window->m_secondPos >= ScreenOptionModeStr[optionMode].count())
        {
            m_window->m_secondPos = 0;
        }
    }
    else
    {
        m_window->m_thirdPos += direction;
        if (m_window->m_thirdPos < 0)
        {
            m_window->m_thirdPos = ScreenOptionModeStr[subCellMode].count() - 1;
        }
        else if (m_window->m_thirdPos >= ScreenOptionModeStr[subCellMode].count())
        {
            m_window->m_thirdPos = 0;
        }
    }
}

void KeyEventHandler::handleCustomKey(QKeyEvent* event)
{
    auto optionMode = Config::instance()->getOptionMode();
    auto subCellMode = Config::instance()->getSubCellOptionMode();
    auto curKeysInUse = ScrrenOptionModeKey[optionMode];
    auto curKeyInCellUse = ScrrenOptionModeKey[subCellMode];
    auto curKey = static_cast<Qt::Key>(event->key());

    if (curKeysInUse.contains(curKey))
    {
        if (m_window->m_firstPos < 0)
        {
            m_window->m_firstPos = curKeysInUse.indexOf(curKey);
        }
        else if (m_window->m_secondPos < 0)
        {
            m_window->m_secondPos = curKeysInUse.indexOf(curKey);
            onKeyMoveMouse();
        }
        else if (m_window->m_thirdPos < 0)
        {
            m_window->m_thirdPos = curKeyInCellUse.indexOf(curKey);
            onKeyMoveMouse();
            m_window->resetStatus();
        }
    }
}

void KeyEventHandler::onKeyMoveMouse()
{
    auto optionMode = Config::instance()->getOptionMode();
    auto subCellMode = Config::instance()->getSubCellOptionMode();
    auto curKeysInUse = ScrrenOptionModeKey[optionMode];
    auto curKeyInCellUse = ScrrenOptionModeKey[subCellMode];

    if (m_window->m_firstPos>=0&&m_window->m_secondPos>=0)
    {
        auto screenGeometry = m_window->currentScreen()->geometry();
        auto xDistance = screenGeometry.width()/curKeysInUse.count();
        auto yDistance = screenGeometry.height()/curKeysInUse.count();
        auto curRect = QRect(m_window->m_firstPos*xDistance,m_window->m_secondPos*yDistance,
                             xDistance,yDistance);

        if (m_window->m_thirdPos<0)
        {
            auto pos = curRect.center();
            auto actualPos = m_window->mapToGlobal(pos);
            qDebug()<<"mouse to:"<<actualPos <<"origin:"<<pos;
            m_currentMousePosition = actualPos;
            QCursor::setPos(actualPos);

        }
        else
        {
            auto size = m_window->getCellColumnAndRowCount(ScreenOptionModeStr[subCellMode].count()-1,curRect);
            auto xSize = curRect.width()/static_cast<double>(size.width());
            auto ySize = curRect.height()/static_cast<double>(size.height());

            int colum =m_window-> m_thirdPos / size.width();
            int row = m_window->m_thirdPos - colum*size.width();
            auto xPos = curRect.x()+row*xSize + xSize/2.0;
            auto yPos = curRect.y()+colum*ySize + ySize/2.0;
            auto actualPos = m_window->mapToGlobal(QPoint{static_cast<int>(xPos),static_cast<int>(yPos)});
            m_currentMousePosition = actualPos;
            qDebug()<<"mouse to:"<<actualPos << "origin:"<<QPoint{static_cast<int>(xPos),static_cast<int>(yPos)};
            QCursor::setPos(actualPos);
        }
    }
}

