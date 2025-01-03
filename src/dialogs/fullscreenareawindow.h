//
// Created by 58226 on 2025/1/2.
//

#ifndef FULLSCREENAREAWINDOW_H
#define FULLSCREENAREAWINDOW_H
#include <QDialog>
#include "helpers/config.h"

class FullScreenAreaWindow:public QDialog {

public:
    struct CutInfo {
        bool shouldCut;
        bool isHorizontal;
        int strips;
        CutInfo(bool cut = false, bool horizontal = false, int s = 0)
            : shouldCut(cut), isHorizontal(horizontal), strips(s) {}
    };

    QSize getCellColumnAndRowCount(int n, QRect area);
public:
    FullScreenAreaWindow(QWidget* parent= nullptr);

    void setOptionMode(ScreenOptionMode fullScreenMode,ScreenOptionMode subCellMode);
    void resetStatus();
    void setCurrentScreen(QScreen* curScr);
    void setConfig(Config config);

    QScreen* currentScreen();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void onKeyMoveMouse();
    void resizeEvent(QResizeEvent* event) override;
    void processBackspace();

    ScreenOptionMode m_optionMode = ScreenOptionMode::QWER;

    ScreenOptionMode m_subCellMode = ScreenOptionMode::QWER;

    QScreen* m_currentScreen{nullptr};

    Config m_config;

    int m_firstPos = -1;
    int m_secondPos = -1;
    int m_thirdPos = -1;


};

#endif //FULLSCREENAREAWINDOW_H
