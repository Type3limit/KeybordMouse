//
// Created by 58226 on 2025/1/8.
//

#ifndef SCREENSELECTIONMODESETPAGE_H
#define SCREENSELECTIONMODESETPAGE_H
#include <QVBoxLayout>

#include "ElaScrollPage.h"


class CommonSettingPage:public ElaScrollPage {

public:
    CommonSettingPage(QWidget* parent= nullptr);
    ~CommonSettingPage();

    void initUI();
private:
    QVBoxLayout* m_layout;
};



#endif //SCREENSELECTIONMODESETPAGE_H
