//
// Created by FU-QAQ on 2025/4/16.
//

#ifndef SCREENSHOTPAGE_H
#define SCREENSHOTPAGE_H

#include <QWidget>

class ScreenshotPage : public QWidget{
    Q_OBJECT
public:
    explicit ScreenshotPage(QWidget *parent = nullptr);
    void captureFullScreen();

signals:
    void screenshotTaken(const QPixmap& pixmap);

protected:
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap _fullscreenPixmap;  // 全屏截图背景
    QPoint  _startPos;
    QPoint  _endPos;
    bool    _isMousePressed = false;
    int     _borderWidth = 2;
    qreal   _dpr;
};



#endif //SCREENSHOTPAGE_H
