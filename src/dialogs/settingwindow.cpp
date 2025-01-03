//
// Created by 58226 on 2025/1/2.
//

// You may need to build the project (run Qt uic code generator) to get "ui_SettingWindow.h" resolved

#include "settingwindow.h"
#include "ui_SettingWindow.h"


SettingWindow::SettingWindow(QWidget *parent) :
    ElaWindow(parent), ui(new Ui::SettingWindow) {
    ui->setupUi(this);
}

SettingWindow::~SettingWindow() {
    delete ui;
}
