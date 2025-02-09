//
// Created by FU-QAQ on 2024/12/18.
//

#ifndef SYNERGYSPOT_USERPAGE_H
#define SYNERGYSPOT_USERPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaWidget.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_USER_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_USER_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pUserPage(_TYPE_,_INFO1_,_INFO2_) UserPage::getInstance(_TYPE_,_INFO1_,_INFO2_)

class ElaInteractiveCard;
class ElaToolButton;
class ElaPushButton;
class QPushButton;
class ElaText;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;

class SS_API UserPage : public ElaWidget{
public:
    static UserPage * getInstance(UserType type, UserInfo uInfo, GroupInfo gInfo, QWidget*parent = nullptr);
    static void destroyUserPage();

    void setInfo(const UserInfo & info);
    void setInfo(const GroupInfo & info);

    void showAt(const QPoint &pos);
signals:
private:
    UserPage() = default;
    explicit UserPage(UserType type,QWidget *parent = nullptr);
    ~UserPage() override;

protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();

    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;
protected slots:
private:
    // ----------------- UI -----------------
    ElaInteractiveCard * _avatarInfo        = nullptr;
    ElaToolButton      * _likeButton        = nullptr;
    ElaPushButton      * _addFriendButton   = nullptr;
    QPushButton        * _sendMsgButton     = nullptr;
    ElaPushButton      * _callButton        = nullptr;
    ElaPushButton      * _editUserButton    = nullptr;
    ElaText            * _remarkText        = nullptr;
    ElaText            * _signContentText   = nullptr;
    ElaText            * _joinDayText       = nullptr;
    ElaText            * _localInfoText     = nullptr;

    ElaText            * _groupResume       = nullptr;
    ElaPushButton      * _groupNotice       = nullptr;

    QVBoxLayout        * _mainLayout        = nullptr;
    QGridLayout        * _textLayout        = nullptr;
    QHBoxLayout        * _buttonLayout      = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    QString              _remark            = "";
    QString              _signContent       = "";
    QString              _joinDay           = "";
    QString              _localInfo         = "";
    bool                 _isGroup           = false;
    // --------------- BackEnd --------------

    static QMap<UserType,UserPage*> _userObjMap;
};

#endif//SYNERGYSPOT_USERPAGE_H
