//
// Created by 58226 on 2025/1/7.
//

#ifndef SHORTCUTSETTINGPAGE_H
#define SHORTCUTSETTINGPAGE_H

#include "ElaFlowLayout.h"
#include "ElaScrollPage.h"

class HotKeySetWidgetBase;
class QVBoxLayout;

class ShortCutSettingPage : public ElaScrollPage {
    Q_OBJECT

public:
    ShortCutSettingPage(QWidget*parent = nullptr);

    ~ShortCutSettingPage();

    void initUI();

    void showEvent(QShowEvent* event) override;

private:
    bool m_hasInited= false;
    ElaFlowLayout* m_layout = nullptr;
    QList<HotKeySetWidgetBase*> m_widgets;
};



#endif //SHORTCUTSETTINGPAGE_H
