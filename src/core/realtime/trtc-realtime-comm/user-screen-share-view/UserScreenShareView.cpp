//
// Created by FU-QAQ on 2025/4/4.
//

#include "UserScreenShareView.h"
#include "../TRTCCloudCallbackDefaultImpl.h"
#include "UserScreenShareViewPage.h"

UserScreenShareView::UserScreenShareView(QWidget *parent) :
    BaseDialogPage(parent)
{
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    ui_user_screen_share_view_ = std::make_unique<UserScreenShareViewPage>(this);
}

UserScreenShareView::~UserScreenShareView()
{

}

void UserScreenShareView::stopUserScreenShare(std::string userId) {
    getTRTCShareInstance()->stopRemoteView(userId.c_str(), liteav::TRTCVideoStreamTypeSub);
}

void UserScreenShareView::show() {
    ui_user_screen_share_view_->show();
}

void UserScreenShareView::retranslateUi() {
    ui_user_screen_share_view_->retranslateUi();
}