//
// Created by FU-QAQ on 2024/12/12.
//

#include "ConversationPage.h"
#include "msg-bubble-model/MsgBubbleModel.h"
#include "msg-bubble-delegate/MsgBubbleDelegate.h"
#include "msg-bubble-view/MsgBubbleView.h"
#include "group-member-dock/GroupMemberDock.h"
#include "../user-page/UserPage.h"

#include "help.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaDockWidget.h"
#include "ela-widget-tools/ElaPlainTextEdit.h"
#include "ela-widget-tools/ElaMenu.h"
#include "ela-widget-tools/Def.h"

#include <QGridLayout>
#include <QPainter>
#include <QListView>
#include <mutex>
#include <ela-widget-tools/ElaPushButton.h>


class InputWidget : public QWidget{
public:
    explicit InputWidget(QWidget * parent = nullptr);
    ~InputWidget() override;
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
    ElaPlainTextEdit * _inputEditFrame     =   nullptr;
    QPushButton      * _sendButton         =   nullptr;
    ElaToolButton    * _sendModButton      =   nullptr;
    ElaMenu          * _sendMod            =   nullptr;
    QGridLayout      * _inputLayout        =   nullptr;
    // ----------------- UI -----------------
};

InputWidget::InputWidget(QWidget *parent) : QWidget(parent)
{
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

InputWidget::~InputWidget(){}

void InputWidget::initWindow() {
    setContentsMargins(0,0,0,0);
    setMaximumHeight(200);
    setMinimumHeight(150);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    _emojiButton         =      new ElaToolButton(this);
    _screenCutButton     =      new ElaToolButton(this);
    _fileButton          =      new ElaToolButton(this);
    _picButton           =      new ElaToolButton(this);
    _voiceMsgButton      =      new ElaToolButton(this);
    _historyMsgButton    =      new ElaToolButton(this);
    _inputEditFrame      =      new ElaPlainTextEdit(this);
    _sendButton          =      new QPushButton(this);
    _sendModButton       =      new ElaToolButton(this);
    _sendMod             =      new ElaMenu(this);
    _inputLayout         =      new QGridLayout(this);
}

void InputWidget::initEdgeLayout() {
    QHBoxLayout * _sendBtnLayout = new QHBoxLayout;
    _sendBtnLayout->setSpacing(5);
    _sendBtnLayout->addStretch();
    _sendBtnLayout->addWidget(_sendButton);
    _sendBtnLayout->addWidget(_sendModButton);
    _sendBtnLayout->setContentsMargins(0,0,0,0);

    _inputEditFrame->setContentsMargins(10,5,10,5);

    // 7 * 12
    _inputLayout->addWidget(_emojiButton,0,0,1,1);
    _inputLayout->addWidget(_screenCutButton,0,1,1,1);
    _inputLayout->addWidget(_fileButton,0,2,1,1);
    _inputLayout->addWidget(_picButton,0,3,1,1);
    _inputLayout->addWidget(_voiceMsgButton,0,4,1,1);
    _inputLayout->addWidget(_historyMsgButton,0,11,1,1);
    _inputLayout->addWidget(_inputEditFrame,1,0,5,12);
    _inputLayout->addItem(_sendBtnLayout,6,0,1,12);
    _inputLayout->setContentsMargins(10,10,0,0);

    this->setLayout(_inputLayout);
}

void InputWidget::initContent() {
    _emojiButton->setElaIcon(ElaIconType::FaceSmile);
    _emojiButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _screenCutButton->setElaIcon(ElaIconType::Scissors);
    _screenCutButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _fileButton->setElaIcon(ElaIconType::FolderArrowUp);
    _fileButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _picButton->setElaIcon(ElaIconType::FolderImage);
    _picButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _voiceMsgButton->setElaIcon(ElaIconType::Microphone);
    _voiceMsgButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _historyMsgButton->setElaIcon(ElaIconType::ClockThree);
    _historyMsgButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    _inputEditFrame->setObjectName(QString::fromUtf8("_inputEditFrame"));
    _inputEditFrame->setStyleSheet("#_inputEditFrame{border:none;background-color:rgb(242,242,242);}");

    _sendMod->addElaIconAction(ElaIconType::CircleCheck,"按 Enter 发送消息");
    _sendMod->addElaIconAction(ElaIconType::Circle,"按 Ctrl + Enter 发送消息");

    _sendModButton->setIsTransparent(false);
    _sendModButton->setMenu(_sendMod);
    _sendModButton->setFixedHeight(30);
    _sendModButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _sendButton->setText("Send");
    _sendButton->setObjectName(QString::fromUtf8("_sendButton"));
    _sendButton->setStyleSheet("QPushButton{background-color:rgb(  0, 153, 255);color:white;border-radius : 5px;}"
                                  "QPushButton:pressed{background-color:rgb(   0, 141, 235);}"
                                  "QPushButton:hover{background-color:rgb(  0, 141, 255);}");
    _sendButton->setFixedHeight(_sendModButton->height());
    _sendButton->setFixedWidth(50);
    setObjectName(QString::fromUtf8("_inputEditWid"));
    setStyleSheet("#_inputEditWid {border:none;background-color:rgb(242,242,242);border-bottom-left-radius: 30px;}");
}

void InputWidget::initConnectFunc() {
}

ConversationFriendPage::ConversationFriendPage(QWidget *parent)
    : QWidget(parent)
{
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

ConversationFriendPage::~ConversationFriendPage(){}

void ConversationFriendPage::initWindow() {
    setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    // user name
    _callButton          =      new ElaToolButton(this);
    _userNameButton      =      new ElaToolButton(this);
    _videoButton         =      new ElaToolButton(this);
    _createGroupButton   =      new ElaToolButton(this);
    _moreOptionButton    =      new ElaToolButton(this);
    _msgListView         =      new MsgBubbleView(this);
    _msgBubbleDelegate   =      new MsgBubbleDelegate;
    _msgBubbleModel      =      new MsgBubbleModel;
    _toolLayout          =      new QHBoxLayout;
}
void ConversationFriendPage::initEdgeLayout() {
    // toolbar layout
    _toolLayout->addWidget(_userNameButton);
    _toolLayout->addStretch();
    _toolLayout->addWidget(_callButton);
    _toolLayout->addWidget(_videoButton);
    _toolLayout->addWidget(_createGroupButton);
    _toolLayout->addWidget(_moreOptionButton);
    _toolLayout->setContentsMargins(0,0,0,0);

    // toolbar wid to set style
    auto * _toolWid = new QWidget(this);
    _toolWid->setContentsMargins(0,10,0,20);
    _toolWid->setObjectName("_toolWid");
    _toolWid->setLayout(_toolLayout);
    _toolWid->setStyleSheet("#_toolWid {border: none; background-color: rgb(242, 242, 242);border-top-right-radius: 30px;}");

    // set msglist size
    _msgListView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    _msgListView->setMinimumWidth(500);
    _msgListView->setMaximumHeight(1000);
    _msgListView->setMinimumHeight(300);

    // main layout for toolbar and msglist
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(_toolWid);
    mainLayout->addWidget(_msgListView);

    this->setLayout(mainLayout);
    this->setContentsMargins(0,0,0,0);

}
void ConversationFriendPage::initContent() {
    // tool button settings
    _userNameButton->setText("{User Name}");
    _userNameButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _callButton->setElaIcon(ElaIconType::CirclePhone);
    _callButton->setIconSize(QSize(32,32));
    _callButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _videoButton->setElaIcon(ElaIconType::CircleVideo);
    _videoButton->setIconSize(QSize(32,32));
    _videoButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _createGroupButton->setElaIcon(ElaIconType::CommentPlus);
    _createGroupButton->setIconSize(QSize(32,32));
    _createGroupButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _moreOptionButton->setElaIcon(ElaIconType::CircleEllipsis);
    _moreOptionButton->setIconSize(QSize(32,32));
    _moreOptionButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    // TODO: connect end back
    ChatMessage his_1{"00","11","hello",":/message-page/rc-page/img/default-avatar-2.jpg",false};
    ChatMessage his_2{"00","11","哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈",":/message-page/rc-page/img/default-avatar-2.jpg",false};
    ChatMessage my_1{"00","11","hi",":/message-page/rc-page/img/default-avatar-1.jpg",true};
    ChatMessage my_2{"00","11","hi你好哇哇哇啊哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇<img src='qrc:/message-page/rc-page/img/default-avatar-3.jpg' width='150' height='90'/>",":/message-page/rc-page/img/default-avatar-1.jpg",true};
    _msgBubbleModel->addMsg(his_1);
    _msgBubbleModel->addMsg(my_1);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(his_2);
    _msgBubbleModel->addMsg(his_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_1);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_1);

    // view setting
    _msgListView->setObjectName("_msgListView");
    _msgListView->setLayoutMode(QListView::Batched);
    _msgListView->setResizeMode(QListView::Adjust);
    _msgListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _msgListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _msgListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _msgListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _msgListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _msgListView->setBatchSize(20);
    _msgListView->setSpacing(2);
    _msgListView->setWordWrap(true);
    _msgListView->setUniformItemSizes(false);
    _msgListView->setStyleSheet("#_msgListView { background-color: rgb(242, 242, 242); border : none;}");
    _msgListView->setModel(_msgBubbleModel);
    _msgListView->setItemDelegate(_msgBubbleDelegate);
    _msgListView->show();
}
void ConversationFriendPage::initConnectFunc() {
}

ConversationGroupPage::ConversationGroupPage(QWidget *parent)
    : QWidget(parent)
{
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

ConversationGroupPage::~ConversationGroupPage(){}

void ConversationGroupPage::initWindow() {
    setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    // group name
    _callButton          =      new ElaToolButton(this);
    _groupNameButton     =      new ElaToolButton(this);
    _videoButton         =      new ElaToolButton(this);
    _fileOfGroup         =      new ElaToolButton(this);
    _inviteAddButton     =      new ElaToolButton(this);
    _moreOptionButton    =      new ElaToolButton(this);
    _msgListView         =      new MsgBubbleView(this);
    _msgBubbleDelegate   =      new MsgBubbleDelegate;
    _msgBubbleModel      =      new MsgBubbleModel;
    _toolLayout          =      new QHBoxLayout;
}

void ConversationGroupPage::initEdgeLayout() {
    // toolbar layout
    _toolLayout->addWidget(_groupNameButton);
    _toolLayout->addStretch();
    _toolLayout->addWidget(_callButton);
    _toolLayout->addWidget(_videoButton);
    _toolLayout->addWidget(_fileOfGroup);
    _toolLayout->addWidget(_inviteAddButton);
    _toolLayout->addWidget(_moreOptionButton);
    _toolLayout->setContentsMargins(0,0,0,0);

    // toolbar wid to set style
    auto * _toolWid = new QWidget(this);
    _toolWid->setContentsMargins(0,10,0,20);
    _toolWid->setObjectName("_toolWid");
    _toolWid->setLayout(_toolLayout);
    _toolWid->setStyleSheet("#_toolWid {border: none; background-color: rgb(242, 242, 242);border-top-right-radius: 30px;}");

    // set msglist size
    _msgListView->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::MinimumExpanding);
    _msgListView->setMinimumWidth(500);
    _msgListView->setMaximumHeight(1000);
    _msgListView->setMinimumHeight(300);

    // main layout for toolbar and msglist
    auto *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(_toolWid);
    mainLayout->addWidget(_msgListView);

    this->setLayout(mainLayout);
    this->setContentsMargins(0,0,0,0);
}

void ConversationGroupPage::initContent() {
    // tool button settings
    _groupNameButton->setText("{Group Name}");
    _groupNameButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _callButton->setElaIcon(ElaIconType::CirclePhone);
    _callButton->setIconSize(QSize(32,32));
    _callButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _videoButton->setElaIcon(ElaIconType::CircleVideo);
    _videoButton->setIconSize(QSize(32,32));
    _videoButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _fileOfGroup->setElaIcon(ElaIconType::Folder);
    _fileOfGroup->setIconSize(QSize(32,32));
    _fileOfGroup->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _inviteAddButton->setElaIcon(ElaIconType::CommentPlus);
    _inviteAddButton->setIconSize(QSize(32,32));
    _inviteAddButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _moreOptionButton->setElaIcon(ElaIconType::CircleEllipsis);
    _moreOptionButton->setIconSize(QSize(32,32));
    _moreOptionButton->setToolButtonStyle(Qt::ToolButtonIconOnly);

    // TODO: connect end back
    ChatMessage his_1{"10002","五花","hello",":/message-page/rc-page/img/default-avatar-2.jpg",false};
    ChatMessage his_2{"10002","五花","哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈哈",":/message-page/rc-page/img/default-avatar-2.jpg",false};
    ChatMessage my_1{"10001","小柴","hi",":/message-page/rc-page/img/default-avatar-1.jpg",true};
    ChatMessage my_2{"10001","小柴","hi你好哇哇哇啊哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇哇<img src='qrc:/message-page/rc-page/img/default-avatar-3.jpg' width='150' height='90'/>",":/message-page/rc-page/img/default-avatar-1.jpg",true};
    _msgBubbleModel->addMsg(his_1);
    _msgBubbleModel->addMsg(my_1);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(his_2);
    _msgBubbleModel->addMsg(his_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_1);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_2);
    _msgBubbleModel->addMsg(my_1);

    // view setting
    _msgBubbleDelegate->setGroupMode(true);
    _msgListView->setObjectName("_msgListView");
    _msgListView->setLayoutMode(QListView::Batched);
    _msgListView->setResizeMode(QListView::Adjust);
    _msgListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _msgListView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _msgListView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _msgListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _msgListView->setBatchSize(20);
    _msgListView->setSpacing(2);
    _msgListView->setWordWrap(true);
    _msgListView->setUniformItemSizes(false);
    _msgListView->setStyleSheet("#_msgListView { background-color: rgb(242, 242, 242); border : none;}");
    _msgListView->setModel(_msgBubbleModel);
    _msgListView->setItemDelegate(_msgBubbleDelegate);
    _msgListView->show();
}

void ConversationGroupPage::initConnectFunc() {
}

ConversationPage::ConversationPage(ConversationType type,QWidget * parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    auto * _layout = new QVBoxLayout;
    auto * _inputWid = new InputWidget(this);
    _layout->setContentsMargins(0,0,5,0);
    _layout->setSpacing(0);
    if(type == Friend){
        _cfP = new ConversationFriendPage(this);
        _layout->addWidget(_cfP);
        _layout->addWidget(_inputWid);
    }
    else if(type == Group){
        _cgP = new ConversationGroupPage(this);
        auto * listAndInputLayout = new QHBoxLayout;
        listAndInputLayout->setContentsMargins(0,0,0,0);
        listAndInputLayout->setSpacing(0);

        // group member list dock
        _memberOfGroupList = new GroupMemberDock(this);
        _memberOfGroupList->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        _memberOfGroupList->setFixedWidth(120);
        _memberOfGroupList->setMaximumHeight(_inputWid->height());
        _memberOfGroupList->addMember(":/message-page/rc-page/img/default-avatar-1.jpg","10001","小柴");
        _memberOfGroupList->addMember(":/message-page/rc-page/img/default-avatar-2.jpg","10002","五花");
        _memberOfGroupList->addMember(":/message-page/rc-page/img/default-avatar-3.jpg","10003","大黄");
        _memberOfGroupList->setObjectName(QString::fromUtf8("_memberOfGroupList"));
        _memberOfGroupList->setStyleSheet("#_memberOfGroupList {background-color: rgb(242, 242, 242);}");

        listAndInputLayout->addWidget(_inputWid);
        listAndInputLayout->addWidget(_memberOfGroupList);

        _layout->addWidget(_cgP);
        _layout->addItem(listAndInputLayout);

        // connect group member single clicked
        connect(_memberOfGroupList,&GroupMemberDock::sigClickedMember,this,[=](QString ssid) {
            // TODO : send request to server
            // get UserInfo DTO
            QList<UserInfo> _list;
            UserInfo user_1{Myself,10,10000,"10001","小柴","","我是小柴er~",":/message-page/rc-page/img/default-avatar-1.jpg",{"中国"}};
            UserInfo user_2{Strangers,99,1000,"10002","五花","","我是五花啊~",":/message-page/rc-page/img/default-avatar-2.jpg",{"中国","湖北","武汉"}};
            UserInfo user_3{Friends,10213,100,"10003","大黄","","我是大黄，汪汪汪~",":/message-page/rc-page/img/default-avatar-3.jpg",{"美国"}};
            _list.append(user_1);_list.append(user_2);_list.append(user_3);
            foreach(auto user,_list) {
                if (user._ssid == ssid) {
                    UserPage * wid = g_pUserPage(user._type,user);
                    QPoint globalPos = QCursor::pos();
                    QPoint offset(wid->width(),wid->height());
                    wid->showAt(globalPos-offset);
                }
            }
        });
    }
    this->setLayout(_layout);
    this->setObjectName("ConversationPage");
    this->setStyleSheet("#ConversationPage{background-color: rgb(242, 242, 242);border-bottom-left-radius: 30px;border-top-right-radius: 30px;}");
}

ConversationPage::~ConversationPage()
{

}