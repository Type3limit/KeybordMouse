//
// Created by 58226 on 2025/1/8.
//

#ifndef SCREENSELECTIONMODESETPAGE_H
#define SCREENSELECTIONMODESETPAGE_H
#include <QVBoxLayout>

#include "ElaScrollPage.h"


class ScreenSelectionModeSetPage:public ElaScrollPage {

public:
    ScreenSelectionModeSetPage(QWidget* parent= nullptr);
    ~ScreenSelectionModeSetPage();

    void initUI();
private:
    QVBoxLayout* m_layout;
};



#endif //SCREENSELECTIONMODESETPAGE_H
