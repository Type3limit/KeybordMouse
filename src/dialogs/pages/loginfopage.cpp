//
// Created by 58226 on 2025/1/8.
//

#include "loginfopage.h"

#include <QVBoxLayout>

#include "ElaListView.h"
#include "ElaLog.h"

LogInfoPage::LogInfoPage(QWidget* parent)
    :ElaScrollPage(parent)
{
    initUI();
}

LogInfoPage::~LogInfoPage()
{
}

void LogInfoPage::initUI()
{
    setPageTitleSpacing(5);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(u8"日志信息");
    m_layout = new QVBoxLayout(centralWidget);
    m_layout->setContentsMargins(0, 0, 0, 0);


    ElaListView* logView = new ElaListView(this);
    logView->setIsTransparent(true);
    m_logModel = new LogModel(this);
    logView->setModel(m_logModel);
    m_layout->addWidget(logView);
    connect(ElaLog::getInstance(), &ElaLog::logMessage, this, [=](const QString& log) {
        m_logModel->appendLogList(log);
    });
    addCentralWidget(centralWidget);
}
