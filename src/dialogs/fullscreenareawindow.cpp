//
// Created by 58226 on 2025/1/2.
//

#include "fullscreenareawindow.h"

#include <QGuiApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QScreen>
#include <QApplication>

#include "ElaTheme.h"

#include "helpers/mouseeventhelper.h"


FullScreenAreaWindow::FullScreenAreaWindow(QWidget* parent)
    :QDialog(parent)
{
    setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setFocusPolicy(Qt::StrongFocus);
}

void FullScreenAreaWindow::setOptionMode(ScreenOptionMode fullScreenMode, ScreenOptionMode subCellMode)
{
    m_optionMode = fullScreenMode;
    m_subCellMode = subCellMode;
}

QSize FullScreenAreaWindow::getCellColumnAndRowCount(int n, QRect area)
{
    if (area.width()> area.height())
    {
        return {6,4};
    }
    return {4,6};
}


void FullScreenAreaWindow::resetStatus()
{
    m_firstPos =-1;
    m_secondPos = -1;
    m_thirdPos = -1;
}

void FullScreenAreaWindow::setCurrentScreen(QScreen* curScr)
{
    m_currentScreen = curScr;
}

void FullScreenAreaWindow::setConfig(Config config)
{
    m_config = config;
}

QScreen* FullScreenAreaWindow::currentScreen()
{
    if (m_currentScreen==nullptr)
        m_currentScreen = QGuiApplication::primaryScreen();
    return m_currentScreen;
}


void FullScreenAreaWindow::paintEvent(QPaintEvent* event) {
    qDebug()<<"paint start";
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    auto screenGeometry = currentScreen()->geometry();
    auto screenStrInUse = ScreenOptionModeStr[m_optionMode];
    auto cellStrInUse = ScreenOptionModeStr[m_subCellMode];

    int drawCount = screenStrInUse.count();
    int xDistance = screenGeometry.width() / drawCount;
    int yDistance = screenGeometry.height() / drawCount;

    // 填充半透明背景
    painter.fillRect(screenGeometry, QColor(0, 0, 0, 128));

    // 配置基本画笔和字体
    QColor gridColor(255, 255, 255, 128);
    QPen pen(gridColor, 1);
    painter.setPen(pen);

    QFont defaultFont = painter.font();
    QFont smallFont = defaultFont;
    smallFont.setPointSize(6);

    QBrush selectedRowBrush(QColor(60, 179, 113, 128));
    QBrush selectedCellBrush(QColor(152, 251, 152, 128));

    // 绘制主网格线
    for (int i = 0; i <= drawCount; ++i) {
        int x = i * xDistance;
        int y = i * yDistance;
        painter.drawLine(0, y, screenGeometry.width(), y); // 水平线
        painter.drawLine(x, 0, x, screenGeometry.height()); // 垂直线
    }

    // 填充网格和绘制文字
    for (int row = 0; row < drawCount; ++row) {
        int xPos = row * xDistance;
        for (int col = 0; col < drawCount; ++col) {
            int yPos = col * yDistance;
            QRect cellRect(xPos, yPos, xDistance, yDistance);

            // 处理选中的行或单元格
            if (m_firstPos >= 0 && row == m_firstPos) {
                if (m_secondPos < 0) {
                    // 选中整行
                    painter.fillRect(cellRect, selectedRowBrush);
                } else if (col == m_secondPos) {
                    // 选中特定单元格
                    painter.fillRect(cellRect, selectedCellBrush);

                    // 绘制子格
                    QSize subCellSize = getCellColumnAndRowCount(cellStrInUse.count() - 1, cellRect);
                    double xSubSize = cellRect.width() / double(subCellSize.width());
                    double ySubSize = cellRect.height() / double(subCellSize.height());

                    int cellIndex = 0;
                    for (int subRow = 0; subRow < subCellSize.height(); ++subRow) {
                        int subY = yPos + subRow * ySubSize;
                        painter.drawLine(cellRect.left(), subY, cellRect.right(), subY);
                        for (int subCol = 0; subCol < subCellSize.width(); ++subCol) {
                            int subX = xPos + subCol * xSubSize;
                            painter.drawLine(subX, cellRect.top(), subX, cellRect.bottom());

                            QRect subCellRect(subX, subY, xSubSize, ySubSize);
                            painter.setFont(smallFont);
                            painter.drawText(subCellRect, Qt::AlignCenter, cellStrInUse[cellIndex++]);
                        }
                    }
                    painter.setFont(defaultFont);
                }
            }

            // 如果 m_thirdPos 小于 0，则继续绘制默认文字
            if (m_thirdPos < 0) {
                QRect leftHalfRect(xPos, yPos, xDistance / 2, yDistance);
                QRect rightHalfRect(xPos + xDistance / 2, yPos, xDistance / 2, yDistance);
                painter.drawText(leftHalfRect, Qt::AlignCenter, screenStrInUse[row]);
                painter.drawText(rightHalfRect, Qt::AlignCenter, screenStrInUse[col]);
            }
        }
    }
    qDebug<<"paint end";
}



void FullScreenAreaWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        processBackspace();
        event->ignore();
    }
    else if (event->key()== Qt::Key_Enter)
    {
        event->ignore();
    }
    else if (event->key()==Qt::Key_Backspace)
    {
        processBackspace();
        event->ignore();
    }
    else if (event->key()==Qt::Key_Tab&&event->modifiers()==Qt::AltModifier)
    {
        auto screens = QGuiApplication::screens();
        auto curIndex = screens.indexOf(currentScreen());
        curIndex +=1;
        if (curIndex>=screens.count())
        {
            curIndex = 0;
        }
        setCurrentScreen(screens[curIndex]);
        setGeometry(currentScreen()->geometry());

        event->accept();
    }
    else if (event->key()==Qt::Key_Space)
    {
        if (event->modifiers()&Qt::ShiftModifier)
        {
            MouseEventHelper::click(MouseEventHelper::Button::Right);
        }
        else
        {
            MouseEventHelper::click(MouseEventHelper::Button::Left);
        }
        event->accept();
    }
    else if (event->key()==Qt::Key_Left)
    {
        if (event->modifiers()&Qt::ShiftModifier)
        {
            auto screens = QGuiApplication::screens();
            auto curIndex = screens.indexOf(currentScreen());
            curIndex -=1;
            if (curIndex<0)
            {
                curIndex = screens.count()-1;
            }
            setCurrentScreen( screens[curIndex]);
            setGeometry(currentScreen()->geometry());
        }
        else
        {
            if (m_thirdPos<0)
            {
                m_firstPos -= 1;
                if (m_firstPos<0)
                {
                    m_firstPos = ScreenOptionModeStr[m_optionMode].count()-1;
                }

            }
            else
            {
                m_thirdPos -= 1;
                if (m_thirdPos>=ScreenOptionModeStr[m_subCellMode].count())
                {
                    m_thirdPos = ScreenOptionModeStr[m_subCellMode].count()-1;
                }
            }
        }
        event->accept();
    }
    else if (event->key()==Qt::Key_Right)
    {
        if (event->modifiers()&Qt::ShiftModifier)
        {
            auto screens = QGuiApplication::screens();
            auto curIndex = screens.indexOf(currentScreen());
            curIndex +=1;
            if (curIndex>=screens.count())
            {
                curIndex = 0;
            }
            setCurrentScreen(screens[curIndex]);
            setGeometry(currentScreen()->geometry());
        }
        else
        {
            if (m_thirdPos<0)
            {
                m_firstPos += 1;
                if (m_firstPos>=ScreenOptionModeStr[m_optionMode].count())
                {
                    m_firstPos = 0;
                }

            }
            else
            {
                m_thirdPos += 1;
                if (m_thirdPos>=ScreenOptionModeStr[m_subCellMode].count())
                {
                    m_thirdPos = 0;
                }
            }
        }

        event->accept();
    }
    else if (event->key()==Qt::Key_Up)
    {
        if (m_thirdPos<0)
        {
            m_secondPos -= 1;
            if (m_secondPos>=ScreenOptionModeStr[m_optionMode].count())
            {
                m_secondPos = 0;
            }

        }
        else
        {
            m_thirdPos -= 1;
            if (m_thirdPos>=ScreenOptionModeStr[m_subCellMode].count())
            {
                m_thirdPos = 0;
            }
        }
        event->accept();
    }
    else if (event->key()==Qt::Key_Down)
    {
        event->accept();
    }
    else
    {
        auto curKeysInUse = ScrrenOptionModeKey[m_optionMode];
        auto curKeyInCellUse = ScrrenOptionModeKey[m_subCellMode];
        auto curKey = static_cast<Qt::Key>(event->key());
        if (curKeysInUse.contains(static_cast<Qt::Key>(event->key())))
        {
            if (m_firstPos<0)
            {
                m_firstPos = curKeysInUse.indexOf(curKey);
            }
            else if (m_secondPos<0)
            {
                m_secondPos = curKeysInUse.indexOf(curKey);
                onKeyMoveMouse();
            }
            else if (m_thirdPos<0)
            {
                m_thirdPos = curKeyInCellUse.indexOf(curKey);
                onKeyMoveMouse();
                resetStatus();
            }
        }
        QDialog::keyPressEvent(event);
    }
    update();
}


void FullScreenAreaWindow::keyReleaseEvent(QKeyEvent* event)
{
    QDialog::keyReleaseEvent(event);
}

void FullScreenAreaWindow::onKeyMoveMouse()
{
    auto curKeysInUse = ScrrenOptionModeKey[m_optionMode];
    auto curKeyInCellUse = ScrrenOptionModeKey[m_subCellMode];

    if (m_firstPos>=0&&m_secondPos>=0)
    {
        auto screenGeometry = currentScreen()->geometry();
        auto xDistance = screenGeometry.width()/curKeysInUse.count();
        auto yDistance = screenGeometry.height()/curKeysInUse.count();
        auto curRect = QRect(m_firstPos*xDistance,m_secondPos*yDistance,xDistance,yDistance);

        if (m_thirdPos<0)
        {
            auto pos = curRect.center();
            QCursor::setPos(pos);
        }
        else
        {
            auto size = getCellColumnAndRowCount(ScreenOptionModeStr[m_subCellMode].count()-1,curRect);
            auto xSize = curRect.width()/(double)(size.width());
            auto ySize = curRect.height()/(double)(size.height());

            int colum = m_thirdPos / size.width();
            int row = m_thirdPos - colum*size.width();
            auto xPos = curRect.x()+row*xSize + xSize/2.0;
            auto yPos = curRect.y()+colum*ySize + ySize/2.0;
            QCursor::setPos(xPos,yPos);
        }

    }

}

void FullScreenAreaWindow::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
}

void FullScreenAreaWindow::processBackspace()
{
    if (m_thirdPos>=0)
    {
        m_thirdPos = -1;
    }
    else if (m_secondPos>=0)
    {
        m_secondPos = -1;
    }
    else if (m_firstPos>=0)
    {
        m_firstPos = -1;
    }
    else
    {
        close();
    }
}

void FullScreenAreaWindow::showEvent(QShowEvent* event)
{
    qDebug()<<"FullScreenAreaWindow  show";
    QDialog::showEvent(event);
}
