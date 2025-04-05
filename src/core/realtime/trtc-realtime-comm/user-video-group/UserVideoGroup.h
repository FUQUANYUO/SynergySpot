//
// Created by FU-QAQ on 2025/4/4.
//

#ifndef USERVIDEOGROUP_H
#define USERVIDEOGROUP_H
#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <map>

#include "../TRTCCloudCallbackDefaultImpl.h"
#include "../user-video-item/UserVideoItem.h"
#include "../user-screen-share-view/UserScreenShareView.h"
#include "UserVideoGroupPage.h"

class UserVideoGroup:public QWidget,public TRTCCloudCallbackDefaultImpl
{
    Q_OBJECT
public:
    explicit UserVideoGroup(QWidget* parent = nullptr);
    ~UserVideoGroup();

private:
    void setNetworkQosParam(liteav::TRTCVideoQosPreference preference, liteav::TRTCQosControlMode controlMode);
    void muteAllRemoteVideoStreams(bool mute);
    void muteAllRemoteAudio(bool mute);
    void showDebugView(bool show);

    //============= ITRTCCloudCallback start ===============//
    void onRemoteUserEnterRoom (const char *userId) override;
    void onRemoteUserLeaveRoom (const char *userId, int reason) override;
    void onUserVideoAvailable(const char* userId, bool available) override;
    void onUserAudioAvailable(const char* userId, bool available) override;
    void onUserSubStreamAvailable(const char *userId, bool available) override;
    void onUserVoiceVolume(liteav::TRTCVolumeInfo* userVolumes, uint32_t userVolumesCount, uint32_t totalVolume) override;
    //============= ITRTCCloudCallback end =================//

signals:
    void onVolumeEvaluationStateChanged(bool state);

private slots:
    void on_networkModeCb_currentIndexChanged(int index);

    void on_muteAllRemoteAudioCb_clicked(bool checked);

    void on_muteAllRemoteVideoCb_clicked(bool checked);

    void on_openDashBoardCb_clicked(bool checked);

    void on_pushButtonShowRemoteScreenShare_clicked();

    void on_checkBoxVolumeEvaluation_stateChanged(int state);

public:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;

    void setMainRoomId(int mainRoomId);
    void addUserVideoItem(liteav::ITRTCCloud* cloud,int roomId,const char *userId,const VIDEO_ITEM::ViewItemType type);
    void onSubRoomUserEnterRoom(liteav::ITRTCCloud* subCloud,int roomId,std::string userId);
    void onSubRoomUserLeaveRoom(int roomId,std::string userId);
    void onSubRoomUserVideoAvailable(liteav::ITRTCCloud*, int roomId, std::string userId, bool available);
    void onSubRoomUserAudioAvailable(int roomId, std::string userId, bool available);
    void onSubRoomExit(int roomId);
    void handleUserVolume(liteav::TRTCVolumeInfo* userVolumes, uint32_t userVolumesCount, uint32_t totalVolume);

private:
    void removeUserVideoItem(int roomId,const char *userId);
    void removeAllUsers();
    void initView();
    void handleUserVideoAvailable(liteav::ITRTCCloud* cloud, int roomId, const char* userId, bool available);
    void handleUserAudioAvailable(int roomId, const char* userId, bool available);
    void updateRemoteViewsMuteStatus(bool status, VIDEO_ITEM::MuteAllType muteType);

public:
    liteav::TXView getLocalVideoTxView();
    static const int ROW_NUM = 3;

private:
    std::unique_ptr<UserVideoGroupPage> ui_video_group_;
    int main_room_id_ = 0;

    std::vector<UserVideoItem*> visible_user_video_items_;
    std::unique_ptr<UserScreenShareView> user_screen_share_view_;
    QString current_screen_sharing_user_id_;
};

#endif //USERVIDEOGROUP_H
