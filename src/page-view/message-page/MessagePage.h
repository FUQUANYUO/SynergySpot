//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_MESSAGEPAGE_H
#define SYNERGYSPOT_MESSAGEPAGE_H

#include "define.h"

#include <QWidget>
#include "common-data/CommonData.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_MESSAGE_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_MESSAGE_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

class ConversationPage;
class ElaInteractiveCard;
class ElaScrollPage;
class ElaTabWidget;
class ElaDockWidget;
class QVBoxLayout;

#define g_pMessagePage MessagePage::getInstance()

class SS_API MessagePage : public QWidget{
    Q_OBJECT
    // grandson class to link card and set content display
    friend class ConversationPage;
public:
    static MessagePage * getInstance();
    static void destroyMessagePage();

    bool loadCacheMsg(const QList<MessageContentDTO>& caches);
signals:
public slots:
    // only process card ui logic , don't include interaction with conversation page
    void addMsgCard(const MsgCombineDTO& info);
private:
    explicit MessagePage(QWidget *parent = nullptr);
    ~MessagePage() override;

    void addMsgContent(const MessageContentDTO& content);
protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
private:
    // ----------------- UI -----------------
    ElaScrollPage               * _tempMsgList;
    ElaTabWidget                * _conversionWid;
    QVBoxLayout                 * _tempMsgWidLayout;
    QWidget                     * _tempMsgWid;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    QMap<ElaInteractiveCard*,MsgCombineDTO>      _tmpUserMsgList;
    QHash<QString,int>                           _unreadMsgCount;
    QHash<ElaInteractiveCard*,ConversationPage*> _cardLinkPageHash;
    QHash<QString,ElaInteractiveCard*>           _ssidLinkCardHash;
    // --------------- BackEnd --------------
    static MessagePage* _messagePage;
};

#endif//SYNERGYSPOT_MESSAGEPAGE_H
