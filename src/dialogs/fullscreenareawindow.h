//
// Created by 58226 on 2025/1/2.
//

#ifndef FULLSCREENAREAWINDOW_H
#define FULLSCREENAREAWINDOW_H
#include <QDialog>
#include <QEvent>

#include "helpers/config.h"

class KeyEventHandler;

class FullScreenAreaWindow:public QWidget {

public:

    QSize getCellColumnAndRowCount(int n, QRect area);

    FullScreenAreaWindow(QWidget* parent= nullptr);

    void resetStatus();

    QScreen* currentScreen();

    void setCurrentScreen(QScreen* curScr);

    void setConfig();

protected:
    void paintEvent(QPaintEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;

    void keyReleaseEvent(QKeyEvent* event) override;

    void closeEvent(QCloseEvent* event) override;

    QScreen* m_currentScreen{nullptr};

    int m_firstPos = -1;
    int m_secondPos = -1;
    int m_thirdPos = -1;


    QPoint m_dragStartPos{INT_MIN,INT_MIN};
    QPoint m_dragEndPos{INT_MIN,INT_MIN};

    KeyEventHandler* m_keyPressHandler {nullptr};

    friend class KeyEventHandler;
};

inline void FullScreenAreaWindow::closeEvent(QCloseEvent* event)
{
    resetStatus();
    QWidget::closeEvent(event);
}


#endif //FULLSCREENAREAWINDOW_H
