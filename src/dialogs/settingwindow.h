//
// Created by 58226 on 2025/1/2.
//

#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include "ElaWindow.h"


QT_BEGIN_NAMESPACE
namespace Ui { class SettingWindow; }
QT_END_NAMESPACE

class SettingWindow : public ElaWindow {
Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow() override;

private:
    Ui::SettingWindow *ui;
};


#endif //SETTINGWINDOW_H
