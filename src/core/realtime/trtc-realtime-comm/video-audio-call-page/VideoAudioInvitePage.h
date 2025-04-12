//
// Created by FU-QAQ on 2025/4/5.
//

#ifndef VIDEOAUDIOINVITEPAGE_H
#define VIDEOAUDIOINVITEPAGE_H

#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <QtOpenGLWidgets/QOpenGLWidget>

class QToolButton;
class VideoAudioCallPage;

class VideoAudioInvitePage : public QOpenGLWidget {
    Q_OBJECT
signals:
    void sigUserProcessResult(bool isAccept);
public:
    VideoAudioInvitePage(QString curSSID = "",QString targetSSID = "",bool isSender = true);
    ~VideoAudioInvitePage();

    void setHangUpBtnEnable(bool isEnable);

    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();

    void initCameraSrc();
protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void sltUpdateFrame();
private:
    // ----------------- UI -----------------
    QToolButton                *pickUpCallBtn      = nullptr;
    QToolButton                *hangUpCallBtn      = nullptr;
    QLabel                     *avatar             = nullptr;
    QLabel                     *userName           = nullptr;
    QLabel                     *inviteInfo         = nullptr;

    QGridLayout                *userInfoDisplay    = nullptr;
    QHBoxLayout                *btnLayout          = nullptr;
    QVBoxLayout                *mainLayout         = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    QString                     _curUserSSID;
    QString                     _targetUserSSID;
    QTimer                     *_timer;
    bool                        _isSender;
    // --------------- BackEnd --------------
};



#endif //VIDEOAUDIOINVITEPAGE_H
