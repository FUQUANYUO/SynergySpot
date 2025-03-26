//
// Created by FU-QAQ on 2025/3/23.
//

#ifndef NOTICEPAGE_H
#define NOTICEPAGE_H

#include "help.h"

#include <QWidget>
#include <QVBoxLayout>


struct GroupBaseInfoDTO;
struct UserBaseInfoDTO;

class ElaSuggestBox;
class ElaPivot;
class ElaScrollPage;
class ElaInteractiveCard;
class ElaToolButton;
class SSMaskWidget;
class LoadingDialog;

enum NoticeStatus {
    Request,
    Waiting,
    Accepted,
    Rejected
};

class NoticePage : public QWidget {
    Q_OBJECT
public:
    NoticePage();
    ~NoticePage();

signals:
    void sigHideArchPageMaskEffect();
public slots:
    void sltMakeFriendRecord(const UserBaseInfoDTO& dto, NoticeStatus status);
    void sltJoinGroupRecord(const GroupBaseInfoDTO& dto, NoticeStatus status);

    void sltShowMaskEffect();
    void sltHideMaskEffect();
    void sltShowLoading();
    void sltHideLoading();

protected:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();

    void paintEvent(QPaintEvent *event) override;
private:
    // ----------------- UI -----------------
    ElaToolButton       * _closeBtn            = nullptr;

    // 用户
    ElaScrollPage       * _noticeScroll        = nullptr;
    QWidget             * _noticeWidget        = nullptr;
    QVBoxLayout         * _noticeLayout        = nullptr;

    QVBoxLayout         * _mainLayout          = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    int                                        _waitToReloadCount = 0;
    int                                        _sigUpdateCount    = 0;
    QMap<QString,ElaInteractiveCard*>          _noticeCardMap;
    // --------------- BackEnd --------------

    SSMaskWidget  *_maskWidget;
    LoadingDialog *_loadingDialog;
};



#endif //NOTICEPAGE_H
