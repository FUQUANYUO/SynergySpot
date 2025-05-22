//
// Created by FU-QAQ on 2024/12/12.
//

#ifndef SYNERGYSPOT_CONVERSATIONPAGE_H
#define SYNERGYSPOT_CONVERSATIONPAGE_H

#include <QTextDocument>
#include <QWidget.h>
#include <QTextEdit>
#include "common-data/common-dto/CommonDatabaseDTO.h"
#include "define.h"

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
class EmojiPickerPage;
class SSTextEdit;

class ConversationPage;

struct ChatMessage;
struct MsgCardInfo;

// input text edit
class SSTextEdit : public QTextEdit {
    Q_OBJECT
signals:
    void sigSendMsgTrigger();
public:
    SSTextEdit(QWidget *parent = nullptr);

    QMap<QString, QImage>& getImageTmpMap();
    void setSendMsgStrategy(bool isEnterToSendMsg);
protected:
    // image from paste board
    void insertFromMimeData(const QMimeData *source) override;

    // drag image
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    // backspace event
    void keyPressEvent(QKeyEvent *event) override;

    // insert logic default scale is 0.3
    void insertImage(const QImage &image, double scale = 0.3);
private:
    QMap<QString, QImage>   _imagesTmpMap;    // pic name without suffix : pic pixmap
    bool                    _isEnterToSendMsg     =   true;
};

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
    ElaToolButton    * _emojiButton          =   nullptr;
    ElaToolButton    * _screenCutButton      =   nullptr;
    ElaToolButton    * _fileButton           =   nullptr;
    ElaToolButton    * _picButton            =   nullptr;
    ElaToolButton    * _voiceMsgButton       =   nullptr;
    ElaToolButton    * _historyMsgButton     =   nullptr;
    SSTextEdit       * _inputEditFrame       =   nullptr;
    QPushButton      * _sendButton           =   nullptr;
    ElaToolButton    * _sendModButton        =   nullptr;
    ElaMenu          * _sendMod              =   nullptr;
    QAction          * _enterStrategy        =   nullptr;
    QAction          * _ctrlAndEnterStrategy =   nullptr;
    QGridLayout      * _inputLayout          =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    EmojiPickerPage  * _emojiPickerPage      =   nullptr;
    // --------------- BackEnd --------------
};

class ConversationFriendPage : public QWidget {
    Q_OBJECT
signals:
    // user page to call video
    void sigCallVideoByOtherButton();
public:
    explicit ConversationFriendPage(const UserBaseInfoDTO& userInfo,QWidget * parent = nullptr);
    ~ConversationFriendPage() override;

    // scroll to bottom
    void scrollMsgViewToBottom();

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
    ElaToolButton    * _videoButton        =   nullptr;
    ElaToolButton    * _createGroupButton  =   nullptr;
    ElaToolButton    * _moreOptionButton   =   nullptr;
    MsgBubbleView    * _msgListView        =   nullptr;
    MsgBubbleDelegate* _msgBubbleDelegate  =   nullptr;
    QHBoxLayout      * _toolLayout         =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    UserBaseInfoDTO    _userInfo;                           // cur conversation user info
    MsgBubbleModel   * _msgBubbleModel     =   nullptr;
    // --------------- BackEnd --------------
};

class ConversationGroupPage : public QWidget{
    Q_OBJECT
public:
    explicit ConversationGroupPage(
        const GroupBaseInfoDTO& groupBaseInfo,
        const QList<GroupMemberInfoDTO>& groupMemberInfo,
        QWidget * parent = nullptr
    );
    ~ConversationGroupPage() override;

    // scroll to bottom
    void scrollMsgViewToBottom();

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
    // ElaToolButton    * _videoButton        =   nullptr;
    ElaToolButton    * _fileOfGroup        =   nullptr;
    ElaToolButton    * _inviteAddButton    =   nullptr;
    ElaToolButton    * _moreOptionButton   =   nullptr;
    MsgBubbleView    * _msgListView        =   nullptr;
    MsgBubbleDelegate* _msgBubbleDelegate  =   nullptr;
    QHBoxLayout      * _toolLayout         =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    GroupBaseInfoDTO          _groupBaseInfo;
    QList<GroupMemberInfoDTO> _groupMemberInfo;
    MsgBubbleModel   *        _msgBubbleModel     =   nullptr;
    UserType                  _curType;
    // --------------- BackEnd --------------
};

enum ConversationType{
    Temp   = 0,
    Friend = 1,
    Group  = 2
};


class ConversationPage : public QWidget{
public:
    explicit ConversationPage(ConversationType type,const MsgCombineDTO& dto ,QWidget * parent);
    ~ConversationPage();

    void scrollMsgViewToBottom();

    QWidget * getConversationTypePage() {
        if (_cfP == nullptr)
            return _cgP;
        else
            return _cfP;
    }
private:
    ConversationFriendPage * _cfP                =   nullptr;
    ConversationGroupPage  * _cgP                =   nullptr;
    GroupMemberDock        * _memberOfGroupList  =   nullptr;
};

#endif//SYNERGYSPOT_CONVERSATIONPAGE_H
