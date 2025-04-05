//
// Created by FU-QAQ on 2025/4/4.
//

#include "VideoAudioCallPage.h"

#include <QVBoxLayout>
#include "CommonData.h"
#include "RoomInfoHolder.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "user-video-item/UserVideoItem.h"

#include <sstream>

VideoAudioCallPage::VideoAudioCallPage(QString curSSID, QString targetSSID, int roomId) {
    // TODO: from server
    SDKAppID = g_pCommonData->getYamlNode()["trtc-api"]["sdkAppId"].as<int>();
    sdkSecret = g_pCommonData->getYamlNode()["trtc-api"]["sdkSecretKey"].as<std::string>();
    getTRTCShareInstance()->addCallback(this);

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();

    moveToCenter();

    enterRoom(roomId, curSSID.toStdString(), TRTCAppSceneVideoCall);
}

VideoAudioCallPage::~VideoAudioCallPage() {
    getTRTCShareInstance()->addCallback(this);
}

void VideoAudioCallPage::initWindow() {
    curUserVideoHolder = new UserVideoItem(
        this,
        getTRTCShareInstance(),
        room_id_,
        curUserSSID,
        VIDEO_ITEM::LocalView);
    micControlBtn      = new ElaToolButton(this);
    cameraControlBtn   = new ElaToolButton(this);
    screenSharedBtn    = new ElaToolButton(this);
    hangUpCallBtn      = new ElaToolButton(this);

    btnLayout          = new QHBoxLayout;
    videoLayout        = new QHBoxLayout;
    mainLayout         = new QVBoxLayout;
}

void VideoAudioCallPage::initEdgeLayout() {
    setMinimumSize(QSize(800,600));

    micControlBtn->setFixedSize(QSize(65,70));
    cameraControlBtn->setFixedSize(QSize(65,70));
    screenSharedBtn->setFixedSize(QSize(65,70));
    hangUpCallBtn->setFixedSize(QSize(65,70));

    curUserVideoHolder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    videoLayout->addWidget(curUserVideoHolder);
    videoLayout->setContentsMargins(0,0,0,0);

    btnLayout->addStretch();
    btnLayout->addWidget(micControlBtn);
    btnLayout->addWidget(cameraControlBtn);
    btnLayout->addWidget(screenSharedBtn);
    btnLayout->addWidget(hangUpCallBtn);
    btnLayout->addStretch();
    btnLayout->setContentsMargins(20,20,20,30);
    btnLayout->setSpacing(30);

    mainLayout->addItem(videoLayout);
    mainLayout->addItem(btnLayout);
    setLayout(mainLayout);
}

void VideoAudioCallPage::initContent() {
    setWindowTitle("音视频通话");
    micControlBtn->setBorderRadius(15);
    cameraControlBtn->setBorderRadius(15);
    screenSharedBtn->setBorderRadius(15);
    hangUpCallBtn->setBorderRadius(15);

    micControlBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    cameraControlBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    screenSharedBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    hangUpCallBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    micControlBtn->setIcon(QPixmap(":/realtime/rc-page/img/microphone_normal.png"));
    cameraControlBtn->setIcon(QPixmap(":/realtime/rc-page/img/camera_normal.png"));
    screenSharedBtn->setIcon(QPixmap(":/realtime/rc-page/img/screen-shared.png"));
    hangUpCallBtn->setIcon(QPixmap(":/realtime/rc-page/img/hang-up.png"));

    micControlBtn->setIconSize(QSize(40,40));
    cameraControlBtn->setIconSize(QSize(40,40));
    screenSharedBtn->setIconSize(QSize(40,40));
    hangUpCallBtn->setIconSize(QSize(40,40));

    micControlBtn->setText("关闭麦克风");
    cameraControlBtn->setText("关闭视频");
    screenSharedBtn->setText("屏幕共享");
    hangUpCallBtn->setText("退出通话");
}

void VideoAudioCallPage::initConnectFunc() {
    connect(micControlBtn,&ElaToolButton::clicked,this,[=]() {
        if (isCloseMicrophone) {
            isCloseMicrophone = false;
            micControlBtn->setIcon(QPixmap(":/realtime/rc-page/img/microphone_normal.png"));
            micControlBtn->setIconSize(QSize(40,40));
            micControlBtn->setText("关闭麦克风");
        }else {
            isCloseMicrophone = true;
            micControlBtn->setIcon(QPixmap(":/realtime/rc-page/img/microphone_close.png"));
            micControlBtn->setIconSize(QSize(40,40));
            micControlBtn->setText("打开麦克风");
        }
        curUserVideoHolder->updateAVMuteStatus(isCloseMicrophone, VIDEO_ITEM::MuteAudio,VIDEO_ITEM::LocalView);
    });
    connect(cameraControlBtn,&ElaToolButton::clicked,this,[=]() {
        if (isCloseCamera) {
            isCloseCamera = false;
            cameraControlBtn->setIcon(QPixmap(":/realtime/rc-page/img/camera_normal.png"));
            cameraControlBtn->setIconSize(QSize(40,40));
            cameraControlBtn->setText("关闭视频");
        }else {
            isCloseCamera = true;
            cameraControlBtn->setIcon(QPixmap(":/realtime/rc-page/img/camera_close.png"));
            cameraControlBtn->setIconSize(QSize(40,40));
            cameraControlBtn->setText("打开视频");
        }
        curUserVideoHolder->updateAVMuteStatus(isCloseCamera, VIDEO_ITEM::MuteVideo,VIDEO_ITEM::RemoteView);
    });
    connect(screenSharedBtn,&ElaToolButton::clicked,this,[=]() {

    });
    connect(hangUpCallBtn,&ElaToolButton::clicked,this,[=]() {
        exitRoom();
        qApp->quit();
    });
}

void VideoAudioCallPage::enterRoom(
    uint32_t roomId,std::string userId,
    liteav::TRTCAppScene appScene, liteav::TRTCRoleType roleType)
{
    room_id_ = roomId;
    curUserSSID = userId;
    app_scene_ = appScene;
    role_type_ = roleType;

    std::ostringstream streamid_os;
    streamid_os << SDKAppID << "_" << room_id_ << "_" << curUserSSID << "_" << "main";
    stream_id_ = streamid_os.str();

    liteav::TRTCParams params;
    params.sdkAppId = SDKAppID;
    params.userId = curUserSSID.c_str();
    params.userSig = GenerateUserSig::genUserSig(params.userId, SDKAppID, sdkSecret.c_str());
    params.roomId = room_id_;
    params.role = roleType;
    params.streamId = stream_id_.c_str();

    getTRTCShareInstance()->enterRoom(params, appScene);
}

void VideoAudioCallPage::exitRoom() {
    getTRTCShareInstance()->exitRoom();
}

void VideoAudioCallPage::onEnterRoom(int result) {
    if (result > 0) {
        // Enable audio
        getTRTCShareInstance()->enableAudioVolumeEvaluation(300); // Effective before the calling of startLocalAudio
        getTRTCShareInstance()->startLocalAudio(liteav::TRTCAudioQualityDefault);

        // Enable video
        if(app_scene_ == liteav::TRTCAppScene::TRTCAppSceneVideoCall || app_scene_ == liteav::TRTCAppScene::TRTCAppSceneLIVE){
            getTRTCShareInstance()->setBeautyStyle(liteav::TRTCBeautyStyleSmooth, 6, 6, 6);
            getTRTCShareInstance()->startLocalPreview(reinterpret_cast<liteav::TXView>(curUserVideoHolder->getVideoWId()));
        }

        RoomInfoHolder::GetInstance().setMainRoomId(room_id_);
        RoomInfoHolder::GetInstance().setUserId(curUserSSID);
        RoomInfoHolder::GetInstance().setCDNPublishStreamId(stream_id_);
        RoomInfoHolder::GetInstance().setMixTranscodingStreamId(stream_id_);
    } else {
        getTRTCShareInstance()->exitRoom();
    }
}

void VideoAudioCallPage::onExitRoom(int reason) {
    curUserVideoHolder->close();
    RoomInfoHolder::GetInstance().resetData();
}

void VideoAudioCallPage::onRemoteUserEnterRoom(const char *userId) {
    // 当前业务场景下有且只有两个用户
    LOG_INFO("RoomState: onRemoteUserEnterRoom(userId:" << userId);

    targetVideoHolder = new UserVideoItem(
        this,getTRTCShareInstance(),
        room_id_,userId,VIDEO_ITEM::RemoteView
    );
    targetVideoHolder->show();
    targetVideoHolder->raise();
    RoomInfoHolder::GetInstance().addRemoteUser(userId);
}

void VideoAudioCallPage::onRemoteUserLeaveRoom(const char *userId, int reason) {
    LOG_INFO("RoomState: onRemoteUserLeaveRoom(userId:" << userId);
    RoomInfoHolder::GetInstance().removeRemoteUser(userId);
}

void VideoAudioCallPage::onUserVideoAvailable(const char* userId, bool available) {
    LOG_INFO("RoomState: onUserVideoAvailable(userId:" << userId << ",available:" << available);
    if(std::strcmp(userId, targetVideoHolder->getUserId().c_str()) == 0 && room_id_ == targetVideoHolder->getRoomId()) {
        if(available && getTRTCShareInstance() != nullptr) {
            getTRTCShareInstance()->startRemoteView(userId, liteav::TRTCVideoStreamType::TRTCVideoStreamTypeBig, reinterpret_cast<liteav::TXView>(targetVideoHolder->getVideoWId()));
        }
        targetVideoHolder->updateAVAvailableStatus(available, isCloseCamera, VIDEO_ITEM::MuteVideo);
    }
}

void VideoAudioCallPage::onUserAudioAvailable(const char *userId, bool available) {
    LOG_INFO("RoomState: onUserAudioAvailable(userId:" << userId << ",available:" << available);
    if(std::strcmp(userId, targetVideoHolder->getUserId().c_str()) == 0 && room_id_ == targetVideoHolder->getRoomId()) {
        targetVideoHolder->updateAVAvailableStatus(available, isCloseMicrophone, VIDEO_ITEM::MuteAudio);
    }
}

void VideoAudioCallPage::onUserVoiceVolume(liteav::TRTCVolumeInfo *userVolumes, uint32_t userVolumesCount, uint32_t totalVolume) {
    TRTCCloudCallbackDefaultImpl::onUserVoiceVolume(userVolumes, userVolumesCount, totalVolume);
}


void VideoAudioCallPage::onSwitchRole(TXLiteAVError errCode, const char *errMsg) {
    TRTCCloudCallbackDefaultImpl::onSwitchRole(errCode, errMsg);
}