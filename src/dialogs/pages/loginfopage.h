//
// Created by 58226 on 2025/1/8.
//

#ifndef LOGINFOPAGE_H
#define LOGINFOPAGE_H
#include <QVBoxLayout>

#include "ElaScrollPage.h"
#include "logmodel.h"


class LogInfoPage:public ElaScrollPage {
public:
    LogInfoPage(QWidget* parent = nullptr);
    ~LogInfoPage();

    void initUI();

private:
    QVBoxLayout* m_layout;
    LogModel * m_logModel;
};



#endif //LOGINFOPAGE_H
