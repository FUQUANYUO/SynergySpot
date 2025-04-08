/**
 * Sub-control for user display
 *
 * - Managing the audio/video status of a single user
 * -
 * - Muting/Unmuting local and remote users:  muteAudio
 * - Stopping/Starting video for local and remote users:  muteVideo
 * - Setting rendering parameters for local and remote videos, including rotation, mirror, and the fill mode:  setRenderParams
 * - Setting the type of remote image to display:  Three image types are supported: big image (TRTCVideoStreamTypeBig), small image (TRTCVideoStreamTypeSmall), and substream image (screen sharing, TRTCVideoStreamTypeSub)
 */

#ifndef USERVIDEOITEM_H
#define USERVIDEOITEM_H

#include <QWidget>
#include "ui_UserVideoItem.h"
#include "../TRTCCloudCallbackDefaultImpl.h"

class QLabel;

namespace VIDEO_ITEM {
    // Image type
    enum ViewItemType
    {
        LocalView,
        RemoteView,
        ScreenSharingView
    };
    // Media to disable
    enum MuteAllType{
        MuteAudio,
        MuteVideo
    };
}

class UserVideoItem:public QWidget,public TRTCCloudCallbackDefaultImpl{
    Q_OBJECT

public:
    UserVideoItem(QWidget *parent = nullptr,
                      liteav::ITRTCCloud* cloud = nullptr,
                      int roomid = 0,
                      std::string userid = nullptr,
                      VIDEO_ITEM::ViewItemType type = VIDEO_ITEM::ViewItemType::RemoteView);
    ~UserVideoItem();
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    void muteAudio(bool mute);
    void muteVideo(bool mute);
    void setRenderParams();
    void setRemoteVideoStreamType();
    virtual void updateDynamicTextUI();
public:
    void updateAVMuteView(VIDEO_ITEM::MuteAllType muteType);
    void setVolume(int volume);

    WId getVideoWId();
    std::string& getUserId();
    VIDEO_ITEM::ViewItemType getViewType();

    void setUserName(const QString& username);

    int getRoomId();
    bool getAudioMuteStatus();
    bool getVideoMuteStatus();
    void updateAVMuteStatus(bool mute, VIDEO_ITEM::MuteAllType muteType,VIDEO_ITEM::ViewItemType viewType);
    void updateAVAvailableStatus(bool available, bool mute_all_remote, VIDEO_ITEM::MuteAllType muteType);
    void initViews();
    void changeEvent(QEvent* event);
private:
    std::unique_ptr<Ui::UserVideoItem> ui_video_item_;
    VIDEO_ITEM::ViewItemType           viewtype_;
    ITRTCCloud*                        trtccloud_;

    int                                room_id_;
    std::string                        user_id_;

    bool                               audio_available_     = false;
    bool                               video_available_     = false;
    bool                               audio_mute_          = false;
    bool                               video_mute_          = false;

    TRTCVideoRotation                  rotation_            = TRTCVideoRotation0;
    TRTCVideoFillMode                  fill_mode_           = TRTCVideoFillMode_Fit;
    TRTCVideoMirrorType                mirror_type_         = TRTCVideoMirrorType_Disable;
    TRTCVideoStreamType                video_stream_type_   = TRTCVideoStreamTypeBig;
};

#endif // USERVIDEOITEM_H