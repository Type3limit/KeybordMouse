//
// Created by 58226 on 2025/1/2.
//

#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include "ElaWindow.h"

class SettingWindow : public ElaWindow {
Q_OBJECT

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow() override;

    void initUI();
};


#endif //SETTINGWINDOW_H
