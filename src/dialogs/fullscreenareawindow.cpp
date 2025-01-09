//
// Created by 58226 on 2025/1/2.
//

#include "fullscreenareawindow.h"

#include <QGuiApplication>
#include <QPainter>
#include <QKeyEvent>
#include <QScreen>
#include <QApplication>
#include <QDebug>
#include <QPainterPath>

#include "ElaTheme.h"
#include "helpers/clickthroughwindow.h"
#include "helpers/keypresshandler.h"

#include "helpers/mouseeventhelper.h"


FullScreenAreaWindow::FullScreenAreaWindow(QWidget* parent)
    :QWidget(parent)
{

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(false);
    m_keyPressHandler = KeyEventHandler::instance();
    m_keyPressHandler->setWindow(this);
    ClickThroughWindow::enableClickThrough(this);
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

void FullScreenAreaWindow::setConfig()
{
    m_keyPressHandler->registHotKeys();
}

QScreen* FullScreenAreaWindow::currentScreen()
{
    if (m_currentScreen==nullptr)
        m_currentScreen = QGuiApplication::primaryScreen();
    return m_currentScreen;
}


void FullScreenAreaWindow::paintEvent(QPaintEvent* event) {

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    auto screenGeometry = currentScreen()->geometry();
    auto screenStrInUse = ScreenOptionModeStr[Config::instance()->getOptionMode()];
    auto cellStrInUse = ScreenOptionModeStr[Config::instance()->getSubCellOptionMode()];

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

    //绘制拖拽位置
    if (m_dragStartPos.x() != INT_MIN && m_dragStartPos.y() != INT_MIN )
    {
        auto curStartPos = mapFromGlobal(m_dragStartPos);
        painter.setPen(QPen(QColor(124,252,0,128), 2));
        if (m_dragEndPos.x() != INT_MIN && m_dragEndPos.y() != INT_MIN)
        {
            auto curEndPos = mapFromGlobal(m_dragEndPos);
            painter.drawRect(QRect{ curStartPos,curEndPos });
        }
        else
        {
            QPainterPath path;
            path.addEllipse(curStartPos,5,5);
            painter.fillPath(path,QColor(124,252,0,128));
        }
    }
}



void FullScreenAreaWindow::keyPressEvent(QKeyEvent* event)
{
    m_keyPressHandler->handleKeyPress(event);
    update();
}

void FullScreenAreaWindow::keyReleaseEvent(QKeyEvent* event)
{
    QWidget::keyReleaseEvent(event);
}
