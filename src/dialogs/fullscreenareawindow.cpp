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


void FullScreenAreaWindow::paintEvent(QPaintEvent* event)
{

    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform|QPainter::TextAntialiasing);

    auto screenGeometry = currentScreen()->geometry();

    auto screenStrInUse = ScreenOptionModeStr[m_optionMode];

    auto cellStrInUse = ScreenOptionModeStr[m_subCellMode];

    painter.fillRect(screenGeometry,QColor::fromRgba(qRgba(0,0,0,128)));

    //绘制方格背景
    auto drawCount = screenStrInUse.count();

    auto xDistance = screenGeometry.width()/drawCount;
    auto yDistance = screenGeometry.height()/drawCount;
    QColor color(255, 255, 255, 128);  // 红色，alpha值128为50%透明度

    auto fontname = painter.font().family();
    QPen pen(color);
    pen.setWidth(1);
    painter.setFont(QFont(fontname,12));

    for (int i = 0;i<drawCount;i++)//横向
    {
        auto yPos = i*yDistance;
        painter.drawLine(0,yPos,screenGeometry.width(),yPos);
        for (int j = 0;j<drawCount;j++)
        {
            auto xPos = j*xDistance;
            painter.setPen(pen);
            painter.drawLine(xPos,0,xPos,screenGeometry.height());
        }
    }
    QBrush fillBrush(QColor::fromRgba(qRgba(152,251,152,128)));
    //填充文字
    for (int row = 0;row<drawCount;row++)
    {

        auto xPos = row*xDistance;
        if (m_firstPos>=0&&m_secondPos<0&&row == m_firstPos)
        {

            auto curRect = QRect(xPos,0,xDistance,screenGeometry.height());
            QBrush rowFillBrush(QColor(60,179,113,128));
            painter.fillRect(curRect,rowFillBrush);
        }
        for (int column = 0;column<drawCount;column++)
        {
            auto yPos = column*yDistance;
            if (m_firstPos>=0 && m_secondPos>=0 && row==m_firstPos&& column==m_secondPos)
            {
                auto curRect = QRect(xPos,yPos,xDistance,yDistance);
                painter.fillRect(curRect,fillBrush);
                //填充子格
                auto size = getCellColumnAndRowCount(screenStrInUse[m_subCellMode].count()-1,curRect);
                auto xSize = curRect.width()/(double)(size.width());
                auto ySize = curRect.height()/(double)(size.height());
                int cellIndex = 0;
                for (int cellCol = 0 ; cellCol<size.height();cellCol++)
                {

                    auto cellY = yPos+cellCol*ySize;
                    painter.drawLine(curRect.x(),cellY,curRect.x()+xDistance,cellY);

                    for (int cellRow = 0;cellRow < size.width();cellRow++)
                    {
                        auto cellX = xPos+cellRow*xSize;
                        painter.drawLine(cellX,curRect.y(),cellX,yDistance+curRect.y());
                        auto cellRect = QRect(cellX,cellY,xSize,ySize);

                        painter.setFont(QFont(fontname,6));
                        painter.drawText(cellRect,Qt::AlignCenter,cellStrInUse[cellIndex++]);
                        painter.setFont(QFont(fontname,12));
                    }
                }

            }
            else
            {
                auto curLeftRect = QRect{xPos,yPos,xDistance / 2,yDistance};
                auto curRightRect = QRect{xPos+xDistance/2,yPos,xDistance/2,yDistance};
                painter.drawText(curLeftRect,Qt::AlignCenter,screenStrInUse[row]);
                painter.drawText(curRightRect,Qt::AlignCenter,screenStrInUse[column]);
            }
        }
    }
}

void FullScreenAreaWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        close();
        event->accept();
    }
    else if (event->key()== Qt::Key_Enter)
    {
        close();
        event->ignore();
    }
    else if (event->key()==Qt::Key_Backspace)
    {
        if (m_thirdPos>=0)
        {
            m_thirdPos = -1;
        }
        else if (m_secondPos >=0)
        {
            m_secondPos = -1;
        }
        else if (m_firstPos>=0)
        {
            m_firstPos = -1;
        }
        event->accept();
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
