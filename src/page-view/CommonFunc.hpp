#ifndef SYNERGYSPOT_COMMONFUNC_HPP
#define SYNERGYSPOT_COMMONFUNC_HPP

#pragma once

#include <QScreen>
#include <QGuiApplication>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>

namespace ComFunc {
    // move the widget to the screen's center
    inline void moveToCenter(QWidget *wid) {
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        int x = (screenGeometry.width() - wid->width()) / 2;
        int y = (screenGeometry.height() - wid->height()) / 2;
        wid->move(x, y);
    }

    // must combine with qt concurrent
    inline void playAudio(const QString &audioPath)
    {
        QMediaPlayer player;
        QAudioOutput output;
        player.setAudioOutput(&output);
        player.setSource(audioPath);
        player.play();
        QEventLoop loop; // 保持线程存活直到播放结束
        QObject::connect(&player, &QMediaPlayer::playbackStateChanged, &loop, [&]() {
            if (player.playbackState() == QMediaPlayer::StoppedState) loop.quit();
        });
        loop.exec();
    }

}
#endif