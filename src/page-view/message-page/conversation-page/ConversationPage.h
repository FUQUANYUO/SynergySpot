//
// Created by FU-QAQ on 2024/12/12.
//

#ifndef SYNERGYSPOT_CONVERSATIONPAGE_H
#define SYNERGYSPOT_CONVERSATIONPAGE_H

#include <QWidget.h>

class ElaToolButton;
class ElaDockWidget;
class QListView;
class QHBoxLayout;
class MsgBubbleModel;
class MsgBubbleDelegate;
class MsgBubbleView;
class GroupMemberDock;

class ConversationFriendPage : public QWidget {
public:
    explicit ConversationFriendPage(QWidget * parent = nullptr);
    ~ConversationFriendPage() override;
protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
private:
    // ----------------- UI -----------------
    ElaToolButton    * _userNameButton     =   nullptr;
    ElaToolButton    * _callButton         =   nullptr;
    ElaToolButton    * _videoButton        =   nullptr;
    ElaToolButton    * _createGroupButton  =   nullptr;
    ElaToolButton    * _moreOptionButton   =   nullptr;
    MsgBubbleView    * _msgListView        =   nullptr;
    MsgBubbleDelegate* _msgBubbleDelegate  =   nullptr;
    QHBoxLayout      * _toolLayout         =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    MsgBubbleModel   * _msgBubbleModel     =   nullptr;
    // --------------- BackEnd --------------
};


class ConversationGroupPage : public QWidget{
public:
    explicit ConversationGroupPage(QWidget * parent = nullptr);
    ~ConversationGroupPage() override;
protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
private:
    // ----------------- UI -----------------
    ElaToolButton    * _groupNameButton    =   nullptr;
    ElaToolButton    * _callButton         =   nullptr;
    ElaToolButton    * _videoButton        =   nullptr;
    ElaToolButton    * _fileOfGroup        =   nullptr;
    ElaToolButton    * _inviteAddButton    =   nullptr;
    ElaToolButton    * _moreOptionButton   =   nullptr;
    MsgBubbleView    * _msgListView        =   nullptr;
    MsgBubbleDelegate* _msgBubbleDelegate  =   nullptr;
    QHBoxLayout      * _toolLayout         =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    MsgBubbleModel   * _msgBubbleModel     =   nullptr;
    // --------------- BackEnd --------------
};

enum ConversationType{
    Temp   = 0,
    Friend = 1,
    Group  = 2
};


class ConversationPage : public QWidget{
public:
    explicit ConversationPage(ConversationType type,QWidget * parent);
    ~ConversationPage();
private:
    ConversationFriendPage * _cfP                =   nullptr;
    ConversationGroupPage  * _cgP                =   nullptr;
    GroupMemberDock        * _memberOfGroupList  =   nullptr;
};

#endif//SYNERGYSPOT_CONVERSATIONPAGE_H
