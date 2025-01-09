//
// Created by 58226 on 2025/1/8.
//

// You may need to build the project (run Qt uic code generator) to get "ui_HotKeySetWidgetBase.h" resolved

#include "hotkeysetwidgetbase.h"
#include <QKeySequenceEdit>

#include "ElaDxgi.h"
#include "elaiconbutton.h"
#include "ElaThemeAnimationWidget.h"
#include "ui_HotKeySetWidgetBase.h"
#include "usermessagebox.h"
#include "helpers/optionnalchain.h"
#include "ElaKeyBinder.h"
#include "ElaLineEditStyle.h"
#include "ElaScrollBar.h"


HotKeySetWidgetBase::HotKeySetWidgetBase(QWidget *parent) :
                                                          QWidget(parent),
                                                          ui(new Ui::HotKeySetWidgetBase) {
    ui->setupUi(this);
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setUniformItemSizes(true);
    ui->listWidget->setSpacing(3);

    auto addSequenceButton = new ElaIconButton(ElaIconType::Plus,this);
    addSequenceButton->setFixedSize(32,32);
    auto removeSequenceButton = new ElaIconButton(ElaIconType::Minus,this);
    removeSequenceButton->setFixedSize(32,32);
    auto resetSequenceButton = new ElaIconButton(ElaIconType::ArrowsRotateReverse,this);
    resetSequenceButton->setFixedSize(32,32);

    connect(addSequenceButton,&QPushButton::clicked,this,[&](){HotKeySetWidgetBase::addWidget({});});
    connect(removeSequenceButton,&QPushButton::clicked,this,&HotKeySetWidgetBase::removeWidget);
    connect(resetSequenceButton,&QPushButton::clicked,this,&HotKeySetWidgetBase::resetSequence);

    ui->buttonHLayout->addWidget(addSequenceButton);
    ui->buttonHLayout->addWidget(removeSequenceButton);
    ui->buttonHLayout->addWidget(resetSequenceButton);

    ui->listWidget->setVerticalScrollBar(new ElaScrollBar(ui->listWidget));
    ui->listWidget->setHorizontalScrollBar(new ElaScrollBar(ui->listWidget));
}

HotKeySetWidgetBase::~HotKeySetWidgetBase() {
    delete ui;
}

void HotKeySetWidgetBase::setHotKeyEditType(HotKeyTypes type)
{
    m_type = type;

    auto sequence = Config::instance()->getHotkeySequence(type);

    ui->m_titleLabel->setText(Config::instance()->getHotkeyDisplayName(type));
    ui->m_titleLabel->setTextStyle(ElaTextType::BodyStrong);
    ui->listWidget->clear();

    m_sequenceEdits.clear();

    for (const auto& itr : sequence)
    {
       addWidget(itr);
    }
}
void HotKeySetWidgetBase::addWidget(const QKeySequence& sequence)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    item->setSizeHint({0,30});
    ui->listWidget->addItem(item);
    auto keySequenceEdit = new QKeySequenceEdit(sequence);
    auto font = keySequenceEdit->font();
    font.setPointSize(14);
    keySequenceEdit->setFont(font);
    keySequenceEdit->setMinimumHeight(30);
    keySequenceEdit->setSizePolicy(QSizePolicy::Policy::Expanding,QSizePolicy::Policy::MinimumExpanding);
    keySequenceEdit->setStyle(new ElaLineEditStyle(keySequenceEdit->style()));
    keySequenceEdit->setStyleSheet("QLineEdit{background-color:#88000000; border:1px solid #e0e0e0;border-radius:4px;}");
    m_sequenceEdits.push_back(keySequenceEdit);
    ui->listWidget->setItemWidget(item,keySequenceEdit);
}
void HotKeySetWidgetBase::removeWidget()
{
    if (ui->listWidget->currentIndex().isValid())
    {
        auto curItem = ui->listWidget->currentItem();
        auto itemWidget =  ui->listWidget->itemWidget(curItem);
        ui->listWidget->removeItemWidget(curItem);
        m_sequenceEdits.removeAll(itemWidget);
    }
}

void HotKeySetWidgetBase::resetSequence()
{
    Config::instance()->resetHotKeySequence(m_type);
    setHotKeyEditType(m_type);
}

void HotKeySetWidgetBase::onSave()
{
    auto res = from(m_sequenceEdits).select<QKeySequence>([&](const QKeySequenceEdit* itr)
    {
        return itr->keySequence();
    }).get();
    if (res.isEmpty())
    {
        UserMessageBox::warning(nullptr,u8"警告",
            u8"快捷键["+Config::instance()->getHotkeyDisplayName(m_type)+"]不能为空");
        return;
    }
    Config::instance()->setHotkeySequence(m_type,res);
}

