#include "UserVideoGroup.h"

#include <QRect>
#include <QGridLayout>
#include <QDebug>

#include "../RoomInfoHolder.h"

#include <QCheckBox>
#include <QComboBox>

UserVideoGroup::UserVideoGroup(QWidget * parent) :QWidget(parent)
{
    // 设置主布局
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    // 创建并添加UserVideoGroupPage
    ui_video_group_ = std::make_unique<UserVideoGroupPage>(this);
    layout->addWidget(ui_video_group_.get());

    // 连接信号与槽
    connect(ui_video_group_->networkModeCb, &QComboBox::currentIndexChanged,
            this, &UserVideoGroup::on_networkModeCb_currentIndexChanged);
    connect(ui_video_group_->muteAllRemoteAudioCb, &QCheckBox::clicked,
            this, &UserVideoGroup::on_muteAllRemoteAudioCb_clicked);
    connect(ui_video_group_->muteAllRemoteVideoCb, &QCheckBox::clicked,
            this, &UserVideoGroup::on_muteAllRemoteVideoCb_clicked);
    connect(ui_video_group_->openDashBoardCb, &QCheckBox::clicked,
            this, &UserVideoGroup::on_openDashBoardCb_clicked);
    connect(ui_video_group_->pushButtonShowRemoteScreenShare, &QPushButton::clicked,
            this, &UserVideoGroup::on_pushButtonShowRemoteScreenShare_clicked);
    connect(ui_video_group_->checkBoxVolumeEvaluation, &QCheckBox::stateChanged,
            this, &UserVideoGroup::on_checkBoxVolumeEvaluation_stateChanged);
    getTRTCShareInstance()->addCallback(this);
}

UserVideoGroup::~UserVideoGroup() {
    getTRTCShareInstance()->removeCallback(this);
    visible_user_video_items_.clear();
}

void UserVideoGroup::setNetworkQosParam(liteav::TRTCVideoQosPreference preference, liteav::TRTCQosControlMode controlMode) {
    liteav::TRTCNetworkQosParam param;
    param.controlMode = controlMode;
    param.preference = preference;
    getTRTCShareInstance()->setNetworkQosParam(param);
}

void UserVideoGroup::muteAllRemoteVideoStreams(bool mute){
    getTRTCShareInstance()->muteAllRemoteVideoStreams(mute);
}

void UserVideoGroup::muteAllRemoteAudio(bool mute){
    getTRTCShareInstance()->muteAllRemoteAudio(mute);
}

void UserVideoGroup::showDebugView(bool show){
    getTRTCShareInstance()->showDebugView(show);
}

//============= ITRTCCloudCallback start =================//
void UserVideoGroup::onRemoteUserEnterRoom(const char *userId){
    qDebug() << "RoomState: UserVideoGroup::onRemoteUserEnterRoom(userId:" << userId;
    if(RoomInfoHolder::GetInstance().getOtherRoomUserId().compare(userId) == 0){
         addUserVideoItem(getTRTCShareInstance(),RoomInfoHolder::GetInstance().getOtherRoomId(),userId,VIDEO_ITEM::RemoteView);
    }else{
         addUserVideoItem(getTRTCShareInstance(),main_room_id_,userId,VIDEO_ITEM::RemoteView);
    }

    RoomInfoHolder::GetInstance().addRemoteUser(userId);
}

void UserVideoGroup::onRemoteUserLeaveRoom(const char *userId, int reason) {
    qDebug() << "RoomState: UserVideoGroup::onRemoteUserLeaveRoom(userId:" << userId;
    RoomInfoHolder::GetInstance().removeRemoteUser(userId);
    if (visible_user_video_items_.size() == 0) {
        return;
    }

    // User in other room
    if(RoomInfoHolder::GetInstance().getOtherRoomUserId().compare(userId) == 0){
         removeUserVideoItem(RoomInfoHolder::GetInstance().getOtherRoomId(),userId);
    }else{
         removeUserVideoItem(main_room_id_,userId);
    }
}

void UserVideoGroup::onUserVideoAvailable(const char *userId, bool available)
{
    qDebug() << "RoomState: UserVideoGroup::onUserVideoAvailable(userId:" << userId << ",available:" << available;
    handleUserVideoAvailable(getTRTCShareInstance(), main_room_id_, userId, available);
}

void UserVideoGroup::onUserAudioAvailable(const char *userId, bool available)
{
    qDebug() << "RoomState: UserVideoGroup::onUserAudioAvailable(userId:" << userId << ",available:" << available;
    handleUserAudioAvailable(main_room_id_, userId, available);
}

void UserVideoGroup::onUserSubStreamAvailable(const char *userId, bool available)
{
    if (available) {
        current_screen_sharing_user_id_ = userId;
        getTRTCShareInstance()->startRemoteView(userId, liteav::TRTCVideoStreamTypeSub, (liteav::TXView)(user_screen_share_view_->winId()));
        user_screen_share_view_->setWindowTitle(QString("screen share from userId: %1").arg(userId));
        user_screen_share_view_->show();
        user_screen_share_view_->raise();
        ui_video_group_->pushButtonShowRemoteScreenShare->setEnabled(true);
    } else {
        getTRTCShareInstance()->stopRemoteView(userId, liteav::TRTCVideoStreamTypeSub);
        user_screen_share_view_->close();
        current_screen_sharing_user_id_ = "";
        ui_video_group_->pushButtonShowRemoteScreenShare->setEnabled(false);
    }
}

void UserVideoGroup::onUserVoiceVolume(liteav::TRTCVolumeInfo* userVolumes, uint32_t userVolumesCount, uint32_t totalVolume) {
    handleUserVolume(userVolumes, userVolumesCount, totalVolume);
}
//============= ITRTCCloudCallback end ==================//

liteav::TXView UserVideoGroup::getLocalVideoTxView() {
    if (visible_user_video_items_.size() > 0 && visible_user_video_items_[0]->getViewType() == VIDEO_ITEM::LocalView) {
        return reinterpret_cast<liteav::TXView>(visible_user_video_items_[0]->getVideoWId());
    }
    return NULL;
}

void UserVideoGroup::setMainRoomId(int mainRoomId)
{
    main_room_id_ = mainRoomId;
}

void UserVideoGroup::addUserVideoItem(liteav::ITRTCCloud* cloud,int roomId,const char *userId, const VIDEO_ITEM::ViewItemType type){
    std::vector<UserVideoItem*>::const_iterator iterator = visible_user_video_items_.begin();

    while (iterator != visible_user_video_items_.end()) {
        if (std::strcmp((*iterator)->getUserId().c_str(), userId) == 0) {
            return;
        }
        iterator++;
    }

    UserVideoItem* videoItem;

    videoItem = new UserVideoItem(ui_video_group_->mainVideoPlaceHolder,
                                            cloud,
                                            roomId,
                                            userId,
                                            type);
    int current_videos = visible_user_video_items_.size();
    int current_row = current_videos / UserVideoGroup::ROW_NUM;
    int current_colulum = current_videos % UserVideoGroup::ROW_NUM;
    QRect qrect = videoItem->geometry();
    QRect new_qrect(current_colulum * qrect.width(), current_row * qrect.height(), qrect.width(), qrect.height());
    videoItem->setParent(ui_video_group_->mainVideoPlaceHolder);
    videoItem->setGeometry(new_qrect);

    videoItem->show();
    videoItem->raise();
    ui_video_group_->mainVideoPlaceHolder->adjustSize();

    visible_user_video_items_.push_back(videoItem);
}

void UserVideoGroup::removeUserVideoItem(int roomId,const char * userId) {
    std::vector<UserVideoItem*>::iterator iter = visible_user_video_items_.begin();

    int position = 0;
    while (iter != visible_user_video_items_.end()) {
        if (std::strcmp(userId, (*iter)->getUserId().c_str()) == 0
                && roomId ==(*iter)->getRoomId()) {
            break;
        }
        position++;
        iter++;
    }

    if (iter == visible_user_video_items_.end()) {
        return;
    }
    (*iter)->close();
    (*iter)->deleteLater();
    visible_user_video_items_.erase(iter);

    iter = visible_user_video_items_.begin();
    iter += position;

    while (iter != visible_user_video_items_.end()) {
        QRect geometry = (*iter)->geometry();
        int current_left = geometry.left();
        int current_top = geometry.top();

        // row - 1
        if (current_left / geometry.width() == 0) {
            int row_num = current_top / geometry.height() - 1;
            int colum_num = 2;
            QRect new_rect(colum_num * geometry.width(), row_num * geometry.width(), geometry.width(), geometry.height());
            (*iter)->setGeometry(new_rect);
            iter++;
            continue;
        }

        int row_num = current_top / geometry.height();
        int colum_num = current_left / geometry.width() - 1;

        QRect new_rect(colum_num * geometry.width(), row_num * geometry.width(), geometry.width(), geometry.height());
        (*iter)->setGeometry(new_rect);
        iter++;
    }
}

void UserVideoGroup::removeAllUsers()
{
    for(std::vector<UserVideoItem*>::iterator iter = visible_user_video_items_.begin(); iter != visible_user_video_items_.end(); iter++) {
        (*iter)->close();
        (*iter)->deleteLater();
    }
    visible_user_video_items_.clear();
}

void UserVideoGroup::on_networkModeCb_currentIndexChanged(int index) {
    liteav::TRTCVideoQosPreference preference = liteav::TRTCVideoQosPreference::TRTCVideoQosPreferenceSmooth;
    switch (index) {
    case 0:
        preference = liteav::TRTCVideoQosPreference::TRTCVideoQosPreferenceClear;
        break;
    case 1:
        preference = liteav::TRTCVideoQosPreference::TRTCVideoQosPreferenceSmooth;
        break;
    }

    liteav::TRTCQosControlMode control_mode = liteav::TRTCQosControlMode::TRTCQosControlModeServer;
    setNetworkQosParam(preference, control_mode);
}

void UserVideoGroup::initView() {
    addUserVideoItem(getTRTCShareInstance(),main_room_id_,"myself", VIDEO_ITEM::LocalView);
    liteav::TRTCVideoQosPreference preference = liteav::TRTCVideoQosPreference::TRTCVideoQosPreferenceSmooth;
    int index = ui_video_group_->networkModeCb->currentIndex();
    switch (index) {
    case 0:
        preference = liteav::TRTCVideoQosPreference::TRTCVideoQosPreferenceClear;
        break;
    case 1:
        preference = liteav::TRTCVideoQosPreference::TRTCVideoQosPreferenceSmooth;
        break;
    }
    ui_video_group_->checkBoxVolumeEvaluation->setChecked(true);
    ui_video_group_->muteAllRemoteAudioCb->setChecked(false);
    ui_video_group_->muteAllRemoteVideoCb->setChecked(false);
    liteav::TRTCQosControlMode control_mode = liteav::TRTCQosControlMode::TRTCQosControlModeServer;
    setNetworkQosParam(preference, control_mode);
}

void UserVideoGroup::handleUserVideoAvailable(liteav::ITRTCCloud* cloud, int roomId, const char* userId, bool available)
{
    for(std::vector<UserVideoItem*>::const_iterator iterator = visible_user_video_items_.begin(); iterator != visible_user_video_items_.end(); iterator++) {
        if(std::strcmp(userId, (*iterator)->getUserId().c_str()) == 0 && roomId == (*iterator)->getRoomId()) {
            if(available && cloud != nullptr) {
                cloud->startRemoteView(userId, liteav::TRTCVideoStreamType::TRTCVideoStreamTypeBig, reinterpret_cast<liteav::TXView>((*iterator)->getVideoWId()));
            }
            bool mute_all_remote_video = ui_video_group_->muteAllRemoteVideoCb->isChecked();
            (*iterator)->updateAVAvailableStatus(available, mute_all_remote_video, VIDEO_ITEM::MuteVideo);
            break;
        }
    }
}

void UserVideoGroup::handleUserAudioAvailable(int roomId, const char *userId, bool available)
{
    for(std::vector<UserVideoItem*>::const_iterator iterator = visible_user_video_items_.begin(); iterator != visible_user_video_items_.end(); iterator++) {
        if(std::strcmp(userId, (*iterator)->getUserId().c_str()) == 0 && roomId == (*iterator)->getRoomId()) {
            bool mute_all_remote_audio = ui_video_group_->muteAllRemoteAudioCb->isChecked();
            (*iterator)->updateAVAvailableStatus(available, mute_all_remote_audio, VIDEO_ITEM::MuteAudio);
            break;
        }
    }
}

void UserVideoGroup::updateRemoteViewsMuteStatus(bool status, VIDEO_ITEM::MuteAllType muteType) {
    for (auto video_item : visible_user_video_items_) {
        if(video_item->getViewType() == VIDEO_ITEM::ViewItemType::RemoteView) {
            video_item->updateAVMuteStatus(status, muteType);
        }
    }
}

void UserVideoGroup::on_muteAllRemoteAudioCb_clicked(bool checked) {
    muteAllRemoteAudio(checked);
    updateRemoteViewsMuteStatus(checked, VIDEO_ITEM::MuteAudio);
}

void UserVideoGroup::on_muteAllRemoteVideoCb_clicked(bool checked) {
    muteAllRemoteVideoStreams(checked);
    updateRemoteViewsMuteStatus(checked, VIDEO_ITEM::MuteVideo);
}

void UserVideoGroup::on_openDashBoardCb_clicked(bool checked) {
    showDebugView(checked);
}

void UserVideoGroup::on_pushButtonShowRemoteScreenShare_clicked()
{
    if(!current_screen_sharing_user_id_.isEmpty() && !user_screen_share_view_->isVisible()) {
        std::string user_id = current_screen_sharing_user_id_.toStdString();
        user_screen_share_view_->stopUserScreenShare(user_id);
        getTRTCShareInstance()->startRemoteView(user_id.c_str(), liteav::TRTCVideoStreamTypeSub, (liteav::TXView)(user_screen_share_view_->winId()));
        user_screen_share_view_->show();
        user_screen_share_view_->raise();
    }
}

void UserVideoGroup::on_checkBoxVolumeEvaluation_stateChanged(int state)
{
    getTRTCShareInstance()->enableAudioVolumeEvaluation((state == Qt::CheckState::Checked)? 300:0);
    emit onVolumeEvaluationStateChanged(state == Qt::CheckState::Checked);
    if(state != Qt::CheckState::Checked) {
        for (auto video_item : visible_user_video_items_) {
            video_item->setVolume(0);
        }
    }
}

void UserVideoGroup::closeEvent(QCloseEvent* event)
{
    removeAllUsers();
    current_screen_sharing_user_id_ = "";
    user_screen_share_view_->close();
}

void UserVideoGroup::showEvent(QShowEvent* event)
{
    initView();
}

void UserVideoGroup::onSubRoomUserEnterRoom(liteav::ITRTCCloud* subCloud,int roomId, std::string userId){
    addUserVideoItem(subCloud,roomId,userId.c_str(),VIDEO_ITEM::RemoteView);
}

void UserVideoGroup::onSubRoomUserLeaveRoom(int roomId, std::string userId){
    removeUserVideoItem(roomId,userId.c_str());
}

void UserVideoGroup::onSubRoomUserVideoAvailable(liteav::ITRTCCloud* subCloud, int roomId, std::string userId, bool available)
{
    const char* user_id = userId.c_str();
    handleUserVideoAvailable(subCloud, roomId, user_id, available);
}

void UserVideoGroup::onSubRoomUserAudioAvailable(int roomId, std::string userId, bool available)
{
    const char* user_id = userId.c_str();
    handleUserAudioAvailable(roomId, user_id, available);
}

void UserVideoGroup::onSubRoomExit(int roomId){
    std::vector<UserVideoItem*>::iterator iter = visible_user_video_items_.begin();
    std::vector<std::string> subRoomUsers;

    while (iter != visible_user_video_items_.end()) {
        if (roomId ==(*iter)->getRoomId()) {
            subRoomUsers.push_back((*iter)->getUserId());
        }
        iter++;
    }

    if(visible_user_video_items_.size() == 0){
        return;
    }

    for(auto userItem : subRoomUsers){
        removeUserVideoItem(roomId, userItem.c_str());
    }
}

void UserVideoGroup::handleUserVolume(liteav::TRTCVolumeInfo* userVolumes, uint32_t userVolumesCount, uint32_t totalVolume) {
    for (auto video_item : visible_user_video_items_) {
        for (uint32_t user_volums_index = 0; user_volums_index < userVolumesCount; user_volums_index++) {
            auto user_volum_item = userVolumes + user_volums_index;

            // The userId for local volume (user_volum_item) is empty.
            if (video_item->getViewType() == VIDEO_ITEM::LocalView
                && strlen(user_volum_item->userId) == 0) {
                video_item->setVolume(user_volum_item->volume);
                break;
            }

            if (strcmp(user_volum_item->userId, video_item->getUserId().c_str()) != 0) {
                continue;
            }
            video_item->setVolume(user_volum_item->volume);
            break;
        }
    }
}