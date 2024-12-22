#ifndef SYNERGYSPOT_COMMONFUNC_H
#define SYNERGYSPOT_COMMONFUNC_H

#include <QGuiApplication>
#include <QScreen>
#include <QWidget>

namespace ComFunc {
    // move the widget to the screen's center
    void moveToCenter(QWidget *wid) {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - wid->width()) / 2;
        int y = (screenGeometry.height() - wid->height()) / 2;
        wid->move(x, y);
    }
}
#endif