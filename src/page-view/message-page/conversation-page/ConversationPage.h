//
// Created by FU-QAQ on 2024/12/12.
//

#ifndef SYNERGYSPOT_CONVERSATIONPAGE_H
#define SYNERGYSPOT_CONVERSATIONPAGE_H

#include <QTextDocument>
#include <QWidget.h>

class ElaToolButton;
class ElaDockWidget;
class ElaMenu;
class QListView;
class QPushButton;
class QHBoxLayout;
class QGridLayout;
class MsgBubbleModel;
class MsgBubbleDelegate;
class MsgBubbleView;
class GroupMemberDock;
class SSTextEdit;

class ConversationPage;

struct ChatMessage;
struct MsgCardInfo;

class InputWidget : public QWidget{
    Q_OBJECT
    friend ConversationPage;
public:
    explicit InputWidget(QWidget * parent = nullptr);
    ~InputWidget() override;
    signals:
        void sigSendBtnClicked(const QString& inputHtml);
protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
private:
    // ----------------- UI -----------------
    ElaToolButton    * _emojiButton        =   nullptr;
    ElaToolButton    * _screenCutButton    =   nullptr;
    ElaToolButton    * _fileButton         =   nullptr;
    ElaToolButton    * _picButton          =   nullptr;
    ElaToolButton    * _voiceMsgButton     =   nullptr;
    ElaToolButton    * _historyMsgButton   =   nullptr;
    SSTextEdit       * _inputEditFrame     =   nullptr;
    QPushButton      * _sendButton         =   nullptr;
    ElaToolButton    * _sendModButton      =   nullptr;
    ElaMenu          * _sendMod            =   nullptr;
    QGridLayout      * _inputLayout        =   nullptr;
    // ----------------- UI -----------------
};

class ConversationFriendPage : public QWidget {
    Q_OBJECT
public:
    explicit ConversationFriendPage(QWidget * parent = nullptr);
    ~ConversationFriendPage() override;
public slots:
    void insertMsgBubble(const ChatMessage& msg) const;
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
    Q_OBJECT
public:
    explicit ConversationGroupPage(QWidget * parent = nullptr);
    ~ConversationGroupPage() override;
public slots:
    void insertMsgBubble(const ChatMessage& msg) const;
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
    explicit ConversationPage(ConversationType type,const MsgCardInfo& info ,QWidget * parent);
    ~ConversationPage();
private:
    ConversationFriendPage * _cfP                =   nullptr;
    ConversationGroupPage  * _cgP                =   nullptr;
    GroupMemberDock        * _memberOfGroupList  =   nullptr;
};

#endif//SYNERGYSPOT_CONVERSATIONPAGE_H
