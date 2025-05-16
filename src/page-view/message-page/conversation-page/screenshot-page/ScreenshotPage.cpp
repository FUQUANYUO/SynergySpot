//
// Created by FU-QAQ on 2025/4/16.
//

#include "ScreenshotPage.h"

#include <QApplication>
#include <QGuiApplication>
#include <QTimer>
#include <QMouseEvent>
#include <QPainter>
#include <QClipboard>

ScreenshotPage::ScreenshotPage(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    showFullScreen();
    captureFullScreen();
}

void ScreenshotPage::captureFullScreen() {
    QScreen *screen = QGuiApplication::primaryScreen();
    _dpr = screen->devicePixelRatio();

    // 获取屏幕的逻辑尺寸（系统缩放后的尺寸）
    QSize logicalSize = screen->size();

    // 按逻辑尺寸截图（自动处理物理像素转换）
    _fullscreenPixmap = screen->grabWindow(0, 0, 0, logicalSize.width(), logicalSize.height());
    _fullscreenPixmap.setDevicePixelRatio(_dpr);
}

void ScreenshotPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        _startPos = event->pos();
        _endPos = event->pos();
        _isMousePressed = true;
        update();
    }
}

void ScreenshotPage::mouseMoveEvent(QMouseEvent *event)
{
    if (_isMousePressed) {
        _endPos = event->pos();
        update();
    }
}

void ScreenshotPage::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && _isMousePressed) {
        _endPos = event->pos();
        _isMousePressed = false;

        // 计算逻辑坐标下的选区并归一化
        QRect selection = QRect(_startPos, _endPos).normalized();

        // 转换为物理像素坐标
        QRect physicalSelection(
            selection.x() * _dpr,
            selection.y() * _dpr,
            selection.width() * _dpr,
            selection.height() * _dpr
        );

        QPixmap cropped = _fullscreenPixmap.copy(physicalSelection);

        if (!cropped.isNull()) {
            // 保持设备像素比以确保显示正确
            cropped.setDevicePixelRatio(_dpr);
            QApplication::clipboard()->setPixmap(cropped);
            emit screenshotTaken(cropped);
        }

        close();
    }
}

void ScreenshotPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // 绘制全屏截图背景
    painter.drawPixmap(0,0, _fullscreenPixmap);

    // 绘制半透明蒙版
    painter.fillRect(rect(), QColor(0, 0, 0, 160));

    if (_isMousePressed) {
        QRect selection = QRect(_startPos, _endPos).normalized();

        // 显示原始屏幕内容
        painter.save();
        painter.setClipRect(selection);
        painter.drawPixmap(0,0, _fullscreenPixmap);
        painter.restore();

        // 绘制边框和尺寸信息
        painter.setPen(QPen(Qt::green, _borderWidth));
        painter.drawRect(selection.adjusted(0, 0, -1, -1));

        painter.setPen(Qt::white);
        painter.drawText(
            selection.bottomRight() + QPoint(10, 15),
            QString("%1×%2").arg(selection.width()).arg(selection.height())
        );
    }
}