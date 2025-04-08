//
// Created by FU-QAQ on 2025/4/4.
//

#ifndef VIDEOAUDIOCALLPAGE_H
#define VIDEOAUDIOCALLPAGE_H

#include "ela-widget-tools/ElaWidget.h"
#include "../TRTCCloudCallbackDefaultImpl.h"

class ElaToolButton;
class QVBoxLayout;
class QHBoxLayout;
class UserVideoItem;
class QLabel;

class VideoAudioCallPage : public ElaWidget,public TRTCCloudCallbackDefaultImpl{
    Q_OBJECT
signals:
    void sigVideoHangUp();
public:
    VideoAudioCallPage(QString curSSID = "",QString targetSSID = "", int roomId = -1, QString userSig = "");
    ~VideoAudioCallPage();

    void setUserSig(const QString &userSig);

    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();

    // 音视频主逻辑
    void enterRoom(uint32_t roomId, std::string userId, liteav::TRTCAppScene appScene, liteav::TRTCRoleType roleType = liteav::TRTCRoleType::TRTCRoleAnchor);
    void exitRoom();
protected:
    // 获取当前场景 当前默认为音视频通话
    liteav::TRTCAppScene getCurrentSelectedAppScene();

    void setMainRoomId(int roomId);

    //============= ITRTCCloudCallback start =================//
    void onEnterRoom(int result) override;
    void onExitRoom(int reason) override;
    void onSwitchRole(TXLiteAVError errCode, const char *errMsg) override;

    void onRemoteUserEnterRoom(const char* userId) override;
    void onRemoteUserLeaveRoom(const char* userId, int reason) override;
    void onUserVideoAvailable(const char* userId, bool available) override;
    void onUserAudioAvailable(const char* userId, bool available) override;
    void onUserVoiceVolume(liteav::TRTCVolumeInfo* userVolumes, uint32_t userVolumesCount, uint32_t totalVolume) override;
    //============= ITRTCCloudCallback end ===================//
private:
    // ----------------- UI -----------------
    UserVideoItem              *curUserVideoHolder = nullptr;
    UserVideoItem              *targetVideoHolder  = nullptr;
    ElaToolButton              *micControlBtn      = nullptr;
    ElaToolButton              *cameraControlBtn   = nullptr;
    ElaToolButton              *screenSharedBtn    = nullptr;
    ElaToolButton              *hangUpCallBtn      = nullptr;

    QHBoxLayout                *videoLayout        = nullptr;
    QHBoxLayout                *btnLayout          = nullptr;
    QVBoxLayout                *mainLayout         = nullptr;
    QLabel                     *curUserNameLabel   = nullptr;
    QLabel                     *targetNameLabel    = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    bool                        isActive           = false;
    bool                        isCloseMicrophone  = false;
    bool                        isCloseCamera      = false;
    uint32_t                    room_id_;
    std::string                 curUserSSID;
    std::string                 targetUserSSID;
    TRTCAppScene                app_scene_;
    TRTCRoleType                role_type_;
    std::string                 stream_id_;
    std::string                 userSig_;
    // --------------- BackEnd --------------
};


#endif //VIDEOAUDIOCALLPAGE_H
