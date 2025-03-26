//
// Created by FU-QAQ on 2025/3/16.
//

#ifndef ADDPAGE_H
#define ADDPAGE_H

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

class AddPage : public QWidget {
    Q_OBJECT
public:
    AddPage();
    ~AddPage();

signals:
    void sigHideArchPageMaskEffect();
    void sigAddBtnClicked(const QString& ssid, bool isGroup);
public slots:
    void sltAddUserRes(const UserBaseInfoDTO& dto);
    void sltAddGroupRes(const GroupBaseInfoDTO& dto);

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
    ElaSuggestBox       * _search              = nullptr;
    ElaPivot            * _typeSwitch          = nullptr;

    // 用户
    ElaScrollPage       * _userResScroll       = nullptr;
    QWidget             * _userResWidget       = nullptr;
    QVBoxLayout         * _userResLayout       = nullptr;

    // 群聊
    ElaScrollPage       * _groupResScroll      = nullptr;
    QWidget             * _groupResWidget      = nullptr;
    QVBoxLayout         * _groupResLayout      = nullptr;

    QVBoxLayout         * _mainLayout          = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    int                                        _waitToReloadCount = 0;
    int                                        _sigUpdateCount    = 0;
    QMap<QString,ElaInteractiveCard*>          _userResMap;
    QMap<QString,ElaInteractiveCard*>          _groupResMap;
    // --------------- BackEnd --------------

    SSMaskWidget  *_maskWidget;
    LoadingDialog *_loadingDialog;
};



#endif //ADDPAGE_H
