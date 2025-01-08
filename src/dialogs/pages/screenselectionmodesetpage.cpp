//
// Created by 58226 on 2025/1/8.
//

#include "screenselectionmodesetpage.h"

#include "config.h"
#include "ElaComboBox.h"
#include "ElaMessageBar.h"
#include "ElaText.h"
#include "ElaToggleSwitch.h"

ScreenSelectionModeSetPage::ScreenSelectionModeSetPage(QWidget* parent)
    :ElaScrollPage(parent)
{
    initUI();
}

ScreenSelectionModeSetPage::~ScreenSelectionModeSetPage()
{

}

void ScreenSelectionModeSetPage::initUI()
{
    setPageTitleSpacing(5);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle(u8"按键方案设置");
    m_layout = new QVBoxLayout(centralWidget);
    m_layout->setContentsMargins(20, 0, 20, 0);
    m_layout->setSpacing(15);

    auto mainOptLayout = new  QHBoxLayout();

    ElaText * mainLabel = new ElaText(u8" 主要按键方案:", centralWidget);
    mainLabel->setTextStyle(ElaTextType::TextStyle::BodyStrong);

    ElaComboBox* mainOption = new ElaComboBox(centralWidget);
    mainOption->addItems(ScreenOptionModeDisplayStr);
    mainOption->setCurrentIndex(0);
    mainOptLayout->addWidget(mainLabel);
    mainOptLayout->addWidget(mainOption);


    auto cellOptLayout = new  QHBoxLayout();
    ElaText * cellLabel = new ElaText(u8" 次要按键方案:", centralWidget);
    cellLabel->setTextStyle(ElaTextType::TextStyle::BodyStrong);

    ElaComboBox* cellOption = new ElaComboBox(centralWidget);
    cellOption->addItems(ScreenOptionModeDisplayStr);
    cellOption->setCurrentIndex(0);
    cellOptLayout->addWidget(cellLabel);
    cellOptLayout->addWidget(cellOption);

    auto closeWhenClickLayout = new QHBoxLayout();
    ElaText* closeWhenClickLabel = new ElaText(u8"点击后关闭窗口:",centralWidget);
    closeWhenClickLabel->setTextStyle(ElaTextType::TextStyle::BodyStrong);
    ElaToggleSwitch* closeWhenClickSwitch = new ElaToggleSwitch(centralWidget);
    closeWhenClickSwitch->setIsToggled(Config::instance()->getCloseAfterClick());
    closeWhenClickLayout->addWidget(closeWhenClickLabel);
    closeWhenClickLayout->addWidget(closeWhenClickSwitch);
    connect(closeWhenClickSwitch,&ElaToggleSwitch::toggled,this,[&](bool checked)
    {
        Config::instance()->setCloseAfterClick(checked);
        Config::instance()->writeJson();
        qDebug()<<"modify close after click mode:"<<checked;
ElaMessageBar::success(ElaMessageBarType::PositionPolicy::TopRight,u8"完成",
u8"点击后关闭方案保存成功",2000,this);
    });

    auto closeWhenDragLayout = new QHBoxLayout();
    ElaText* closeWhenDragLabel = new ElaText(u8"拖动后关闭窗口:",centralWidget);
    closeWhenDragLabel->setTextStyle(ElaTextType::TextStyle::BodyStrong);
    ElaToggleSwitch* closeWhenDragSwitch = new ElaToggleSwitch(centralWidget);
    closeWhenDragSwitch->setIsToggled(Config::instance()->getCloseAfterDrag());
    closeWhenDragLayout->addWidget(closeWhenDragLabel);
    closeWhenDragLayout->addWidget(closeWhenDragSwitch);
    connect(closeWhenDragSwitch,&ElaToggleSwitch::toggled,this,[&](bool checked)
    {
        Config::instance()->setCloseAfterDrag(checked);
        Config::instance()->writeJson();
        qDebug()<<"modify close after drag mode:"<<checked;
ElaMessageBar::success(ElaMessageBarType::PositionPolicy::TopRight,u8"完成",
u8"拖拽后关闭方案保存成功",2000,this);
    });

    mainOptLayout->addStretch();
    m_layout->addLayout(mainOptLayout);
    cellOptLayout->addStretch();
    m_layout->addLayout(cellOptLayout);
    closeWhenClickLayout->addStretch();
    m_layout->addLayout(closeWhenClickLayout);
    closeWhenDragLayout->addStretch();
    m_layout->addLayout(closeWhenDragLayout);
    m_layout->addStretch();
    addCentralWidget(centralWidget);

    connect(mainOption, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int index)
    {
        Config::instance()->setOptionMode((ScreenOptionMode)index);
        Config::instance()->writeJson();
        qDebug()<<"modify main option mode:"<<ScreenOptionModeDisplayStr[index];
        ElaMessageBar::success(ElaMessageBarType::PositionPolicy::TopRight,u8"完成",
      u8"主要按键方案保存成功",2000,this);
    });
    connect(cellOption, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [&](int index)
    {
        Config::instance()->setSubCellOptionMode((ScreenOptionMode)index);
        Config::instance()->writeJson();
        qDebug()<<"modify subcell option mode:"<<ScreenOptionModeDisplayStr[index];
        ElaMessageBar::success(ElaMessageBarType::PositionPolicy::TopRight,u8"完成",
     u8"次要按键方案保存成功",2000,this);
    });
}
