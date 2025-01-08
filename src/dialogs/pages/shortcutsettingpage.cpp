//
// Created by 58226 on 2025/1/7.
//

#include "shortcutsettingpage.h"

#include <qframe.h>

#include "ElaFlowLayout.h"
#include "ElaMessageButton.h"
#include "hotkeysetwidgetbase.h"
#include "keypresshandler.h"
#include "mouseeventhelper.h"
#include "helpers/config.h"

ShortCutSettingPage::ShortCutSettingPage(QWidget* parent)
{
    setPageTitleSpacing(5);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(u8"热键设置");

    auto vBox = new QVBoxLayout(centralWidget);
    vBox->setContentsMargins(0, 0, 0, 0);

    m_layout = new ElaFlowLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);

    auto hBoxLayout = new QHBoxLayout();
    hBoxLayout->setContentsMargins(0, 0, 10, 0);
    hBoxLayout->setSpacing(10);
    hBoxLayout->addStretch();

    auto resetButton = new ElaMessageButton(centralWidget);
    resetButton->setText(u8"重置");
    resetButton->setMessageMode(ElaMessageBarType::MessageMode::Success);
    resetButton->setPositionPolicy(ElaMessageBarType::PositionPolicy::BottomRight);
    resetButton->setBarTitle(u8"已全部重置");
    resetButton->setBarText(u8"快捷键已全部重置");

    auto saveButton = new ElaMessageButton(centralWidget);
    saveButton->setText(u8"保存");
    saveButton->setMessageMode(ElaMessageBarType::MessageMode::Success);
    saveButton->setPositionPolicy(ElaMessageBarType::PositionPolicy::BottomRight);
    saveButton->setBarTitle(u8"已保存");
    saveButton->setBarText(u8"快捷键设置已保存,快捷按键已重置");

    connect(saveButton,&QPushButton::clicked,this,[&]()
    {
        for (const auto& itr : m_widgets)
        {
            itr->onSave();
        }
        Config::instance()->writeJson();
        KeyEventHandler::instance()->registHotKeys();
    });

    connect(resetButton,&QPushButton::clicked,this,[&]()
    {
        for (const auto& itr : m_widgets)
        {
            itr->resetSequence();
        }
        Config::instance()->writeJson();
        KeyEventHandler::instance()->registHotKeys();
    });

    hBoxLayout->addWidget(resetButton);

    hBoxLayout->addWidget(saveButton);

    vBox->addLayout(hBoxLayout);
    vBox->addLayout(m_layout);
    addCentralWidget(centralWidget);
}

ShortCutSettingPage::~ShortCutSettingPage()
{
}

void ShortCutSettingPage::initUI()
{
    for (int i = 0 ;i<n_ConfigType;i++)
    {
        auto setWidget = new HotKeySetWidgetBase(this);
        setWidget->setHotKeyEditType(static_cast<ConfigType>(i));
        m_widgets.push_back(setWidget);
        QFrame* line = new QFrame(this);
        line->setFixedHeight(2);
        line->setSizePolicy(QSizePolicy::Policy::Expanding,QSizePolicy::Policy::Fixed);
        line->setStyleSheet("background-color: #E0E0E0;");
        m_layout->addWidget(setWidget);
        m_layout->addWidget(line);
    }
}

void ShortCutSettingPage::showEvent(QShowEvent* event)
{
    if (!m_hasInited)
    {
        m_hasInited = true;
        initUI();
    }
    ElaScrollPage::showEvent(event);
}
