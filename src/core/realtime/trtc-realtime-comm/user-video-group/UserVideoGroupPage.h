//
// Created by FU-QAQ on 2025/4/4.
//

#ifndef USERVIDEOGROUPPAGE_H
#define USERVIDEOGROUPPAGE_H

#include <QWidget>

class QComboBox;
class QPushButton;
class QCheckBox;

class UserVideoGroup;

class UserVideoGroupPage : public QWidget {
   Q_OBJECT
  friend class UserVideoGroup;
public:
    explicit UserVideoGroupPage(QWidget *parent = nullptr);
private:
    // 成员控件
    QComboBox *networkModeCb;
    QCheckBox *muteAllRemoteAudioCb;
    QCheckBox *muteAllRemoteVideoCb;
    QCheckBox *openDashBoardCb;
    QCheckBox *checkBoxVolumeEvaluation;
    QPushButton *pushButtonShowRemoteScreenShare;
    QWidget *mainVideoPlaceHolder;
};

#endif //USERVIDEOGROUPPAGE_H
