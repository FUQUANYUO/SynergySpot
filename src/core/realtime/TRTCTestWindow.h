//
// Created by FU-QAQ on 2025/4/4.
//

// TRTCTestWindow.h
#ifndef TRTCTESTWINDOW_H
#define TRTCTESTWINDOW_H

#include "trtc-realtime-comm/TRTCCloudCallbackDefaultImpl.h"
#include "trtc-realtime-comm/TRTCRealtimeCommHandler.h"
#include <QMainWindow>

#include <QPushButton>

class UserVideoItem;
class UserVideoGroup;
class QLineEdit;

class TRTCTestWindow : public QMainWindow, public TRTCCloudCallbackDefaultImpl
{
    Q_OBJECT

public:
    explicit TRTCTestWindow();
    ~TRTCTestWindow();

    void initUi();

    void enterRoom(uint32_t roomId, std::string userId, liteav::TRTCAppScene appScene, liteav::TRTCRoleType roleType = liteav::TRTCRoleType::TRTCRoleAnchor);
    void exitRoom();

    liteav::TRTCAppScene getCurrentSelectedAppScene();

    //============= ITRTCCloudCallback start =================//
    void onEnterRoom(int result);
    void onExitRoom(int reason);
    void onSwitchRole(TXLiteAVError errCode, const char *errMsg);
    //============= ITRTCCloudCallback end ===================//
private:
    //============= Ui =================//
    QLineEdit * roomId;
    QLineEdit * userId;
    QPushButton * enterRoomButton;
    //============= Ui =================//

    std::shared_ptr<UserVideoGroup> user_video_group_;
    uint32_t room_id_;
    std::string user_id_;
    liteav::TRTCAppScene app_scene_;
    liteav::TRTCRoleType role_type_;
    std::string stream_id_;
};

#endif // TRTCTESTWINDOW_H
