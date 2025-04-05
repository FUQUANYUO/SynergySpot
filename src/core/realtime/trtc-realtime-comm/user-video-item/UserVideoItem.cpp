#include "UserVideoItem.h"

UserVideoItem::UserVideoItem(QWidget * parent,liteav::ITRTCCloud* cloud, int roomid, std::string userid, VIDEO_ITEM::ViewItemType type)
    :QWidget(parent), ui_video_item_(new Ui::UserVideoItem), viewtype_(type) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    this->room_id_ = roomid;
    this->user_id_ = userid;
    this->trtccloud_ = cloud;
    ui_video_item_->setupUi(this);
    ui_video_item_->verticalLayout->setContentsMargins(0,0,0,0);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    initViews();
    if(this->trtccloud_ != nullptr) {
        setRenderParams();
        setRemoteVideoStreamType();
    }
}

UserVideoItem::~UserVideoItem() {
    if(trtccloud_ != nullptr){
       trtccloud_ = nullptr;
    }
}

void UserVideoItem::muteAudio(bool mute) {
    if (viewtype_ == VIDEO_ITEM::LocalView) {
        trtccloud_->muteLocalAudio(mute);
        return;
    }

    if (viewtype_ == VIDEO_ITEM::RemoteView) {
        trtccloud_->muteRemoteAudio(user_id_.c_str(), mute);
        return;
    }

}
void UserVideoItem::muteVideo(bool mute) {
    if (viewtype_ == VIDEO_ITEM::LocalView) {
        trtccloud_->muteLocalVideo(mute);
        return;
    }

    if (viewtype_ == VIDEO_ITEM::RemoteView) {
        trtccloud_->muteRemoteVideoStream(user_id_.c_str(), mute);
        return;
    }
}

void UserVideoItem::setRenderParams() {
    liteav::TRTCRenderParams param;
    param.rotation = rotation_;
    param.fillMode = fill_mode_;
    param.mirrorType = mirror_type_;

    if (viewtype_ == VIDEO_ITEM::LocalView) {
        trtccloud_->setLocalRenderParams(param);
        return;
    }

    if (viewtype_ == VIDEO_ITEM::RemoteView) {
        trtccloud_->setRemoteRenderParams(user_id_.c_str(), liteav::TRTCVideoStreamTypeBig, param);
        return;
    }

}
void UserVideoItem::setRemoteVideoStreamType() {
    trtccloud_->setRemoteVideoStreamType(user_id_.c_str(), video_stream_type_);
}

void UserVideoItem::changeEvent(QEvent* event) {
    if (QEvent::LanguageChange == event->type()) {
        ui_video_item_->retranslateUi(this);
        updateDynamicTextUI();
    }
    QWidget::changeEvent(event);
}

void UserVideoItem::initViews() {
    updateDynamicTextUI();
    switch (viewtype_)
    {
    case VIDEO_ITEM::LocalView:
        audio_available_ = true;
        video_available_ = true;
        break;
    case VIDEO_ITEM::RemoteView:
        updateAVMuteView(VIDEO_ITEM::MuteVideo);
        updateAVMuteView(VIDEO_ITEM::MuteAudio);
        break;
    case VIDEO_ITEM::ScreenSharingView:
        break;
    default:
        break;
    }
}
void UserVideoItem::updateAVMuteStatus(bool mute, VIDEO_ITEM::MuteAllType muteType, VIDEO_ITEM::ViewItemType viewType)
{
    if(muteType == VIDEO_ITEM::MuteAudio){
        audio_mute_ = mute;
        if (viewType == VIDEO_ITEM::LocalView) {
            trtccloud_->muteLocalAudio(mute);
        }else if (viewType == VIDEO_ITEM::RemoteView){
            trtccloud_->muteRemoteAudio(user_id_.c_str(), mute);
        }
    }
    if(muteType == VIDEO_ITEM::MuteVideo){
        video_mute_ = mute;
        if (viewType == VIDEO_ITEM::LocalView) {
            trtccloud_->muteLocalVideo(mute);
        }else if (viewType == VIDEO_ITEM::RemoteView){
            trtccloud_->muteRemoteVideoStream(user_id_.c_str(), mute);
        }
    }
    updateAVMuteView(muteType);
}

void UserVideoItem::updateAVAvailableStatus(bool available, bool mute_all_remote, VIDEO_ITEM::MuteAllType muteType) {
    if (muteType == VIDEO_ITEM::MuteAudio) {
        if (mute_all_remote) {
            audio_mute_ = true;
        }
        audio_available_ = available;
        updateAVMuteView(VIDEO_ITEM::MuteAudio);
    }

    if (muteType == VIDEO_ITEM::MuteVideo) {
        if (mute_all_remote) {
            video_mute_ = true;
        }
        video_available_ = available;
        updateAVMuteView(VIDEO_ITEM::MuteVideo);
    }
}


void UserVideoItem::updateAVMuteView(VIDEO_ITEM::MuteAllType muteType){
    if (muteType == VIDEO_ITEM::MuteAudio) {
        if (audio_mute_) {
            ui_video_item_->volumePb->setValue(0);
        }
    }
}

void UserVideoItem::setVolume(int volume)
{
    ui_video_item_->volumePb->setValue(volume);
}

WId UserVideoItem::getVideoWId()
{
    return ui_video_item_->videoPlaceHolder->winId();
}

std::string& UserVideoItem::getUserId()
{
    return user_id_;
}

int UserVideoItem::getRoomId() {
    return room_id_;
}

bool UserVideoItem::getAudioMuteStatus() {
    return audio_mute_;
}

bool UserVideoItem::getVideoMuteStatus() {
    return video_mute_;
}

VIDEO_ITEM::ViewItemType UserVideoItem::getViewType()
{
    return viewtype_;
}

void UserVideoItem::updateDynamicTextUI() {

}