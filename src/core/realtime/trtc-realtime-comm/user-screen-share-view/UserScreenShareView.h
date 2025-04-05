//
// Created by FU-QAQ on 2025/4/4.
//

#ifndef USERSCREENSHAREVIEW_H
#define USERSCREENSHAREVIEW_H


#include "BaseDialogPage.h"
#include "UserScreenShareViewPage.h"

class UserScreenShareView : public BaseDialogPage
{
  Q_OBJECT

public:
  explicit UserScreenShareView(QWidget *parent = nullptr);
  ~UserScreenShareView();
  void stopUserScreenShare(std::string userId);
  void show();
private:
  void retranslateUi() override;
private:
  std::unique_ptr<UserScreenShareViewPage> ui_user_screen_share_view_;
};


#endif //USERSCREENSHAREVIEW_H
