//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_CONTACTPAGE_H
#define SYNERGYSPOT_CONTACTPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaScrollPage.h"

#include "notice-page/NoticePage.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_CONTACT_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_CONTACT_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif


class ElaToolButton;
class ElaPivot;
class ElaTreeView;
class QVBoxLayout;
class ContactModel;

struct MsgCombineDTO;
struct FriendshipDTO;

#define g_pContactPage ContactPage::getInstance()

class SS_API ContactPage : public ElaScrollPage{
    Q_OBJECT
public:
    static ContactPage * getInstance();
    static void destroyContactPage();

    void addFriendGrouping(const QString& name);

    QList<QString> getFriendGrouping();
    bool loadCacheContact(const QList<FriendshipDTO>& caches);

    // get user model view
    ElaTreeView *getFriendTreeView();

public slots:
    void addContactInfo(const QString& groupingName,const MsgCombineDTO &info);
signals:
    void sigTriggerAddMsgCard(const MsgCombineDTO &info);
    void sigTriggerAddToCreateGroupList(const UserBaseInfoDTO &dto);

    void sigHideArchPageMaskEffect();
    void sigShowArchPageMaskEffect();
    void sigAddMakeFriendRecord(const UserBaseInfoDTO& dto, NoticeStatus status);
    void sigAddJoinGroupRecord(const GroupBaseInfoDTO& dto, NoticeStatus status);

    // by user info display page
    void sigCommunicateRequestBySSID(const QString& ssid);
private:
    explicit ContactPage(QWidget *parent = nullptr);
    ~ContactPage() override;

protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
private:
    // ----------------- UI -----------------
    QWidget       * _centralWidget         = nullptr;
    QVBoxLayout   * _centralWidLayout      = nullptr;
    ElaToolButton * _friendNoticeButton    = nullptr;
    ElaToolButton * _groupNoticeButton     = nullptr;
    ElaPivot      * _friendOrGroupPivot    = nullptr;
    ElaTreeView   * _friendTree            = nullptr;
    ElaTreeView   * _groupTree             = nullptr;
    ContactModel  * _friendModel           = nullptr;
    ContactModel  * _groupModel            = nullptr;

    QVBoxLayout   * _scrollTreeLayout      = nullptr;
    NoticePage    * _noticePage            = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    QMap<QString,QMap<QString,QList<MsgCombineDTO>>>  _groupingInfos;
    // assist find MsgCardInfo
    QHash<QString, MsgCombineDTO>                     _ssidToCardInfoHash;
    // --------------- BackEnd --------------

    static ContactPage* _contactPage;
};

#endif//SYNERGYSPOT_CONTACTPAGE_H
