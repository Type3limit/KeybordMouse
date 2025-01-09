//
// Created by 58226 on 2025/1/2.
//

#ifndef FULLSCREENAREAWINDOW_H
#define FULLSCREENAREAWINDOW_H
#include <QDialog>
#include <QEvent>

#include "helpers/config.h"

class GlobalKeyboardHook;
class KeyEventHandler;

class FullScreenAreaWindow:public QWidget {

public:

    QSize getCellColumnAndRowCount(int n, QRect area);

    FullScreenAreaWindow(QWidget* parent= nullptr);

    void resetStatus();

    QScreen* currentScreen();


    void setCurrentScreen(QScreen* screen);

    void setConfig();

protected:
    void paintEvent(QPaintEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

    void hideEvent(QHideEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    void showEvent(QShowEvent* event) override;

    int m_firstPos = -1;
    int m_secondPos = -1;
    int m_thirdPos = -1;

    QScreen* m_currentScreen{nullptr};

    QPoint m_dragStartPos{INT_MIN,INT_MIN};
    QPoint m_dragEndPos{INT_MIN,INT_MIN};

    KeyEventHandler* m_keyPressHandler {nullptr};

    GlobalKeyboardHook* m_globalKeyboardHook{nullptr};

    friend class KeyEventHandler;
};



#endif //FULLSCREENAREAWINDOW_H
