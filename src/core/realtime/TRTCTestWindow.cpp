#include "TRTCTestWindow.h"

#include "trtc-realtime-comm/RoomInfoHolder.h"
#include "trtc-realtime-comm/user-video-group/UserVideoGroup.h"

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QWidget>
#include <memory>
#include <sstream>

static const int SDKAppID = 1600080428;
static const char* SDKSECRETKEY = "84c94260e0a671ec6ffd3e17364d797222b2184d88a0e502aae8b95d869d2a0b";


TRTCTestWindow::TRTCTestWindow()
{
    user_video_group_ = std::make_shared<UserVideoGroup>();
    initUi();
    getTRTCShareInstance()->addCallback(this);
}

TRTCTestWindow::~TRTCTestWindow() {
    getTRTCShareInstance()->removeCallback(this);
}

void TRTCTestWindow::initUi() {
    setFixedSize(600,500);
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setFixedSize(600,500);
    setCentralWidget(centralWidget);

    // 视频区域
    user_video_group_->setParent(centralWidget);
    // user_video_group_->hide();

    // 输入区域
    QLineEdit* roomId = new QLineEdit(centralWidget);
    QLineEdit* userId = new QLineEdit(centralWidget);
    roomId->setPlaceholderText("房间ID");
    userId->setPlaceholderText("用户ID");

    QPushButton* enterRoomButton = new QPushButton("enter room", centralWidget);

    // 按钮区域布局
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(roomId);
    buttonLayout->addWidget(userId);
    buttonLayout->addWidget(enterRoomButton);

    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(user_video_group_.get(), 1); // 视频区域占剩余空间
    mainLayout->addLayout(buttonLayout);     // 按钮区域固定高度

    connect(enterRoomButton,&QPushButton::clicked, this, [=]() {
        liteav::TRTCAppScene app_scene = getCurrentSelectedAppScene();
        uint32_t room_id = roomId->text().toUInt();
        std::string user_id = userId->text().toStdString();
        if (app_scene == liteav::TRTCAppScene::TRTCAppSceneLIVE || app_scene == liteav::TRTCAppScene::TRTCAppSceneVoiceChatRoom) {
            // int selelct_role_index = ui_mainwindow_->userRoleComB->currentIndex();
            // if (selelct_role_index == -1){
            //     QMessageBox::warning(NULL, "Failed to enter the room", "You must select a role in live streaming scenarios.");
            //     return;
            // }
            // liteav::TRTCRoleType role_type = getCurrentSelectedRoleType();
            // test_base_scene_.enterRoom(room_id, user_id, app_scene, role_type);
        } else {
            enterRoom(room_id, user_id, app_scene);
        }
    });
}

void TRTCTestWindow::enterRoom(uint32_t roomId, std::string userId, liteav::TRTCAppScene appScene, liteav::TRTCRoleType roleType) {

    room_id_ = roomId;
    user_id_ = userId;
    app_scene_ = appScene;
    role_type_ = roleType;

    // Stream ID for CDN live streaming, which is optional and cannot be longer than 64 bytes.
    // To avoid application conflicts, we recommend that you set it to "sdkappid_roomid_userid_main".
    std::ostringstream streamid_os;
    streamid_os << SDKAppID << "_" << room_id_ << "_" << user_id_ << "_" << "main";
    stream_id_ = streamid_os.str();

    liteav::TRTCParams params;
    params.sdkAppId = SDKAppID;
    params.userId = user_id_.c_str();
    /** @note:  Do not use the code below in your commercial application. This is because:
     * The code may be able to calculate UserSig correctly, but it is only for quick testing of the SDK’s basic features, not for commercial applications.
     * SDKSECRETKEY in client code can be easily decompiled and reversed, especially on web.
     * Once your key is disclosed, attackers will be able to steal your Tencent Cloud traffic.
     * The correct method is to deploy the UserSig calculation code and encryption key on your project server so that your application can request from your server a UserSig that is calculated whenever one is needed.
     * Given that it is more difficult to hack a server than a client application, server-end calculation can better protect your key.
     * Documentation:  https://intl.cloud.tencent.com/document/product/647/35166#Server
     */
    params.userSig = GenerateUserSig::genUserSig(params.userId, SDKAppID, SDKSECRETKEY);
    params.role = role_type_;
    params.roomId = room_id_;
    params.streamId = stream_id_.c_str();

    getTRTCShareInstance()->enterRoom(params, appScene);
}

void TRTCTestWindow::exitRoom() {
    getTRTCShareInstance()->exitRoom();
}

liteav::TRTCAppScene TRTCTestWindow::getCurrentSelectedAppScene() {
    liteav::TRTCAppScene appScene = liteav::TRTCAppScene::TRTCAppSceneVideoCall;
    // int current_index = ui_mainwindow_->appSceneComboBox->currentIndex();
    int current_index = 0;
    switch(current_index) {
        case 0:
            appScene = liteav::TRTCAppScene::TRTCAppSceneVideoCall;
        break;
        case 1:
            appScene = liteav::TRTCAppScene::TRTCAppSceneLIVE;
        break;
        case 2:
            appScene = liteav::TRTCAppScene::TRTCAppSceneAudioCall;
        break;
        case 3:
            appScene = liteav::TRTCAppScene::TRTCAppSceneVoiceChatRoom;
        break;
        default:
            break;
    }
    return appScene;
}

//============= ITRTCCloudCallback start===================//

void TRTCTestWindow::onEnterRoom(int result) {
    if (result > 0) {
        user_video_group_->setMainRoomId(room_id_);
        user_video_group_->show();

        // Enable audio
        getTRTCShareInstance()->enableAudioVolumeEvaluation(300); // Effective before the calling of startLocalAudio
        getTRTCShareInstance()->startLocalAudio(liteav::TRTCAudioQualityDefault);

        // Enable video
        if(app_scene_ == liteav::TRTCAppScene::TRTCAppSceneVideoCall || app_scene_ == liteav::TRTCAppScene::TRTCAppSceneLIVE){
            getTRTCShareInstance()->setBeautyStyle(liteav::TRTCBeautyStyleSmooth, 6, 6, 6);
            getTRTCShareInstance()->startLocalPreview(user_video_group_->getLocalVideoTxView());
        }

        RoomInfoHolder::GetInstance().setMainRoomId(room_id_);
        RoomInfoHolder::GetInstance().setUserId(user_id_);
        RoomInfoHolder::GetInstance().setCDNPublishStreamId(stream_id_);
        RoomInfoHolder::GetInstance().setMixTranscodingStreamId(stream_id_);
    } else {
        getTRTCShareInstance()->exitRoom();
    }
}

void TRTCTestWindow::onExitRoom(int reason) {
    user_video_group_->close();
    RoomInfoHolder::GetInstance().resetData();
}

void TRTCTestWindow::onSwitchRole(TXLiteAVError errCode, const char * errMsg){
    if (errCode == TXLiteAVError::ERR_NULL){
        QMessageBox::about(NULL, "Successfully", "Role changed successfully.");
    }else{
        QMessageBox::warning(NULL, "Failed to change the role",errMsg);
    }
}
//============= ITRTCCloudCallback end===================//

