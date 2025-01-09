#ifndef SYNERGYSPOT_COMMONFUNC_HPP
#define SYNERGYSPOT_COMMONFUNC_HPP

#pragma once

#include <QScreen>
#include <QGuiApplication>
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