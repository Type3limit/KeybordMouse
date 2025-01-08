//
// Created by 58226 on 2025/1/8.
//

#ifndef HOTKEYSETWIDGETBASE_H
#define HOTKEYSETWIDGETBASE_H

#include <QKeySequenceEdit>

#include "ElaWidget.h"
#include "helpers/config.h"


QT_BEGIN_NAMESPACE
namespace Ui { class HotKeySetWidgetBase; }
QT_END_NAMESPACE

class HotKeySetWidgetBase : public QWidget {
Q_OBJECT

public:
    explicit HotKeySetWidgetBase(QWidget *parent = nullptr);
    ~HotKeySetWidgetBase() override;

    void setHotKeyEditType(ConfigType type);

    void addWidget(const QKeySequence& sequence);

    void removeWidget();

    void resetSequence();

    void onSave();
private:
    Ui::HotKeySetWidgetBase *ui;
    ConfigType m_type;
    QList<QKeySequenceEdit*> m_sequenceEdits;
};


#endif //HOTKEYSETWIDGETBASE_H
