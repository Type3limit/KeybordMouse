//
// Created by 58226 on 2025/1/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SettingWindow.h" resolved

#include "settingwindow.h"

#include "ElaMessageBar.h"
#include "loginfopage.h"
#include "screenselectionmodesetpage.h"
#include "shortcutsettingpage.h"

SettingWindow::SettingWindow(QWidget *parent) :
    ElaWindow(parent) {
    initUI();
    resize(1280, 760);
}

SettingWindow::~SettingWindow() {

}


void SettingWindow::initUI()
{
    setWindowTitle(u8"设置");
    setWindowIconText(u8"设置");
    setUserInfoCardVisible(false);
    //for elawindow addPageNode need page has content
    addPageNode(u8"按键方案",new ScreenSelectionModeSetPage(this),ElaIconType::PresentationScreen);
    addPageNode(u8"热键设置", new ShortCutSettingPage(this),ElaIconType::Keyboard);
    addPageNode(u8"日志信息",new LogInfoPage(this),ElaIconType::MemoPad);

    ElaMessageBar::success(ElaMessageBarType::PositionPolicy::TopRight,u8"提示",
        u8"窗口初始化成功",5000,this);

    qDebug()<<"SettingWindow init success";
}
