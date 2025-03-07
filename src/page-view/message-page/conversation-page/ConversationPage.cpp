//
// Created by FU-QAQ on 2024/12/12.
//
#include "ConversationPage.h"
#include "msg-bubble-model/MsgBubbleModel.h"
#include "msg-bubble-delegate/MsgBubbleDelegate.h"
#include "msg-bubble-view/MsgBubbleView.h"
#include "group-member-dock/GroupMemberDock.h"
#include "../user-page/UserPage.h"
#include "../MessagePage.h"

#include "help.h"
#include "uuid/GenUUID.h"
#include "common-data/CommonData.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaDockWidget.h"
#include "ela-widget-tools/ElaMenu.h"
#include "ela-widget-tools/Def.h"
#include "ela-widget-tools/ElaInteractiveCard.h"

#include <QGridLayout>
#include <QTextEdit>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QPainter>
#include <QDateTime>
#include <QListView>
#include <mutex>

// input text edit
class SSTextEdit : public QTextEdit {
public:
    SSTextEdit(QWidget *parent = nullptr);

    QMap<QString, QImage>& getImageTmpMap();
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
    QMap<QString, QImage> _imagesTmpMap;    // pic name without suffix : pic pixmap
};

SSTextEdit::SSTextEdit(QWidget *parent): QTextEdit(parent) {
    setAcceptDrops(true);
}

void SSTextEdit::insertFromMimeData(const QMimeData *source) {
    if (source->hasImage()) {
        QImage image = qvariant_cast<QImage>(source->imageData());
        insertImage(image);
    } else if (source->hasUrls()) {
        QList<QUrl> urls = source->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QImage image(url.toLocalFile());
                if (!image.isNull()) {
                    insertImage(image);
                }
            }
        }
    } else {
        QTextEdit::insertFromMimeData(source);
    }
}

void SSTextEdit::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasImage() || event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void SSTextEdit::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasImage()) {
        QImage image = qvariant_cast<QImage>(mimeData->imageData());
        insertImage(image);
    } else if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                QImage image(url.toLocalFile());
                if (!image.isNull()) {
                    insertImage(image);
                }
            }
        }
    }
}

void SSTextEdit::keyPressEvent(QKeyEvent *event) {
    if (document()->isEmpty() && event->key() == Qt::Key_Backspace) {
        _imagesTmpMap.clear();
    }
    QTextEdit::keyPressEvent(event);
}

void SSTextEdit::insertImage(const QImage &image, double scale) {
    QTextCursor cursor = textCursor();
    QTextDocument *document = this->document();

    int width = static_cast<int>(image.width() * scale);
    int height = static_cast<int>(image.height() * scale);

    QString imageName = QString::fromStdString(g_pGenUUID->generateUUID("msg_pic"));
    document->addResource(QTextDocument::ImageResource, QUrl(imageName), QVariant(image));

    QTextImageFormat imageFormat;
    imageFormat.setName(imageName);
    imageFormat.setWidth(width);
    imageFormat.setHeight(height);
    cursor.insertImage(imageFormat);

    _imagesTmpMap.insert(imageName, image);
}

QMap<QString, QImage>& SSTextEdit::getImageTmpMap() {
    return _imagesTmpMap;
}

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
    _inputEditFrame      =      new SSTextEdit(this);
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
    connect(_sendButton, &QPushButton::clicked, this, [=]() {
        if (!_inputEditFrame->document()->isEmpty()) {
            emit sigSendBtnClicked(_inputEditFrame->toHtml());
        }
    });
}

ConversationFriendPage::ConversationFriendPage(const UserBaseInfoDTO& userInfo,QWidget *parent)
    : QWidget(parent)
{
    _userInfo = userInfo;

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

ConversationFriendPage::~ConversationFriendPage(){}

void ConversationFriendPage::insertMsgBubble(const ChatMessage& msg) const {
    _msgBubbleModel->addMsg(msg);
}

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
    _toolLayout->setContentsMargins(10,0,0,0);

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
    _userNameButton->setText(_userInfo.username);
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
    connect(_userNameButton,&QPushButton::clicked,[=]() {
        // TODO: remark and region need to get from server
        UserInfo userInfo{
            Friends,
            static_cast<int>(std::difftime(GetCurTime::getTimeObj()->getCurTimeStamp(),_userInfo.createTime)/ (60 * 60 * 24)) + 1,
            static_cast<int>(_userInfo.thumbUpCount), _userInfo.ssid, _userInfo.username, "", _userInfo.personalSign,
            _userInfo.avatarPath, {""}
        };
        UserPage * wid = g_pUserPage(Friends,userInfo,{});
        QPoint globalPos = QCursor::pos();
        wid->showAt(globalPos + QPoint(10,10));
    });
}

ConversationGroupPage::ConversationGroupPage(
    const GroupBaseInfoDTO& groupBaseInfo,
    const QList<GroupMemberInfoDTO>& groupMemberInfo,
    QWidget *parent
)
    : QWidget(parent)
{
    _groupBaseInfo = groupBaseInfo;
    _groupMemberInfo = groupMemberInfo;

    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

ConversationGroupPage::~ConversationGroupPage(){}

void ConversationGroupPage::insertMsgBubble(const ChatMessage& msg) const {
    _msgBubbleModel->addMsg(msg);
}

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
    _toolLayout->setContentsMargins(10,0,0,0);

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
    _groupNameButton->setText(_groupBaseInfo.groupName);
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
    connect(_groupNameButton,&QPushButton::clicked,[=]() {
        // TODO: remark and region need to get from server
        GroupInfo groupInfo{
            UserType::Groups,_groupBaseInfo.ssidGroup,_groupBaseInfo.groupName,"",
            _groupBaseInfo.profile,_groupBaseInfo.avatarPath,static_cast<int>(_groupMemberInfo.count()),
            {
                {"这是一个公告"}
            }
        };
        UserPage * wid = g_pUserPage(Groups,{},groupInfo);
        QPoint globalPos = QCursor::pos();
        wid->showAt(globalPos + QPoint(10,10));
    });
}

ConversationPage::ConversationPage(ConversationType type,const MsgCombineDTO& dto ,QWidget * parent)
    : QWidget(parent)
{
    setWindowFlag(Qt::FramelessWindowHint);
    auto * _layout = new QVBoxLayout;
    auto * _inputWid = new InputWidget(this);
    _layout->setContentsMargins(0,0,5,0);
    _layout->setSpacing(0);
    QString _curSSID = g_pCommonData->getCurUserInfo().ssid;
    QString _curName = g_pCommonData->getCurUserInfo().username;
    if(type == ConversationType::Friend){
        _cfP = new ConversationFriendPage(
            dto.userBaseInfo,this
        );
        _layout->addWidget(_cfP);
        _layout->addWidget(_inputWid);

        // send msg by myself
        connect(_inputWid, &InputWidget::sigSendBtnClicked, this, [=](const QString& html) {
            QString html_cp = html;
            // add msg pic to the tmp
            QMap<QString,QImage>& cacheImage = _inputWid->_inputEditFrame->getImageTmpMap();
            QList<QString> fileIDs;
            if (!_inputWid->_inputEditFrame->getImageTmpMap().isEmpty()) {
                for (auto imageIt = cacheImage.begin(); imageIt != cacheImage.end(); imageIt++) {
                    std::string imageName = imageIt.key().toStdString();
                    g_pCommonData->addMsgPicToTmp(imageIt.value(),imageName);// store in the tmp dir
                    html_cp.replace(imageIt.key(),QString::fromStdString(g_pCommonData->getDataPath(msgPic) + "/" + imageName + g_pCommonData->getImageEx()));
                    fileIDs.append(imageIt.key());
                }
                cacheImage.clear();
            }
            _inputWid->_inputEditFrame->clear();

            _cfP->insertMsgBubble({_curSSID,_curName,
                        html_cp,g_pCommonData->getCurUserInfo().avatarPath,true});

            // store msg
            qint64 curTimeStamp = GetCurTime::getTimeObj()->getCurTimeStamp();
            MessageContentDTO msgDto{
                _curSSID,
                ContentType::Text,
                html_cp,
                fileIDs,
                {
                    1,
                    dto.userBaseInfo.ssid
                },
                curTimeStamp
            };

            // get grandfather to link card and set content display
            MessagePage * msgPage = dynamic_cast<MessagePage*>(parent->parent());

            // replace image url to [图片] placeholders
            QRegularExpression imgRegex("<img[^>]*>", QRegularExpression::CaseInsensitiveOption);
            html_cp.replace(imgRegex,"[图片]");
            QFont font;
            QTextDocument docu;
            docu.setHtml(html_cp);
            font.setPointSize(9);

            msgPage->_ssidLinkCardHash[dto.userBaseInfo.ssid]->setTimeContent(QString::fromStdString(
                GetCurTime::getTimeObj()->getMsgTypeTime(curTimeStamp)),
                Qt::gray,font);
            msgPage->_ssidLinkCardHash[dto.userBaseInfo.ssid]->setSubTitle(docu.toPlainText());

            // sync with server
            g_pCommonData->setMessageContentData({msgDto});
        });
    }
    else if(type == ConversationType::Group){
        _cgP = new ConversationGroupPage(
            dto.groupBaseInfo,
            dto.groupMemberInfo,
            this
        );
        auto * listAndInputLayout = new QHBoxLayout;
        listAndInputLayout->setContentsMargins(0,0,0,0);
        listAndInputLayout->setSpacing(0);

        // group member list dock
        _memberOfGroupList = new GroupMemberDock(this);
        _memberOfGroupList->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
        _memberOfGroupList->setFixedWidth(120);
        _memberOfGroupList->setMaximumHeight(_inputWid->height());

        // TODO: mark
        QHash<QString,UserBaseInfoDTO> tmpUserHash;
        for (const auto &it : dto.groupMemberInfo ) {
            UserBaseInfoDTO user = g_pCommonData->getUserInfoBySSID(it.ssidMember);
            _memberOfGroupList->addMember(user.avatarPath, user.ssid, user.username );
            tmpUserHash.insert(it.ssidMember,user);
        }

        _memberOfGroupList->setObjectName(QString::fromUtf8("_memberOfGroupList"));
        _memberOfGroupList->setStyleSheet("#_memberOfGroupList {background-color: rgb(242, 242, 242);}");

        listAndInputLayout->addWidget(_inputWid);
        listAndInputLayout->addWidget(_memberOfGroupList);

        _layout->addWidget(_cgP);
        _layout->addItem(listAndInputLayout);

        // connect group member single clicked
        connect(_memberOfGroupList,&GroupMemberDock::sigClickedMember,this,[=](QString ssid) {
            // TODO : send request to server
            UserBaseInfoDTO user = tmpUserHash.value(ssid);
            bool isFriend = g_pCommonData->isCurUserFriend(ssid);

            UserInfo uInfo{
                (isFriend?UserType::Friends:UserType::Strangers),
                static_cast<int>(std::difftime(GetCurTime::getTimeObj()->getCurTimeStamp(),user.createTime) / (60 * 60 * 24) + 1),
                static_cast<int>(user.thumbUpCount) , user.ssid, user.username, "", user.personalSign,
                user.avatarPath,{""}
            };

            UserPage * wid = g_pUserPage((isFriend?UserType::Friends:UserType::Strangers),uInfo,{});

            QPoint globalPos = QCursor::pos();
            QPoint offset(wid->width(),wid->height());
            wid->showAt(globalPos-offset);
        });

        connect(_inputWid, &InputWidget::sigSendBtnClicked, this, [=](const QString& html) {
            QString html_cp = html;
            // add msg pic to the tmp
            QList<QString> fileIds;
            QMap<QString,QImage>& cacheImage = _inputWid->_inputEditFrame->getImageTmpMap();
            if (!_inputWid->_inputEditFrame->getImageTmpMap().isEmpty()) {
                for (auto imageIt = cacheImage.begin(); imageIt != cacheImage.end(); imageIt++) {
                    std::string imageName = imageIt.key().toStdString();
                    g_pCommonData->addMsgPicToTmp(imageIt.value(),imageName);
                    html_cp.replace(imageIt.key(),QString::fromStdString(g_pCommonData->getDataPath(msgPic) + "/" + imageName + g_pCommonData->getImageEx()));
                    fileIds.append(imageIt.key());
                }
                cacheImage.clear();
            }
            _inputWid->_inputEditFrame->clear();
            _cgP->insertMsgBubble({_curSSID,_curName,
                        html_cp,g_pCommonData->getCurUserInfo().avatarPath,true});

            // store msg
            qint64 curTimeStamp = GetCurTime::getTimeObj()->getCurTimeStamp();
            MessageContentDTO msgDto{
                _curSSID,
                ContentType::Text,
                html_cp,
                fileIds,
                {
                    2,
                    dto.groupBaseInfo.ssidGroup
                },
                curTimeStamp
            };

            // get grandfather to link card and set content display
            MessagePage * msgPage = dynamic_cast<MessagePage*>(parent->parent());

            // replace image url to [图片] placeholders
            QRegularExpression imgRegex("<img[^>]*>", QRegularExpression::CaseInsensitiveOption);
            html_cp.replace(imgRegex,"[图片]");
            QFont font;
            QTextDocument docu;
            docu.setHtml(html_cp);
            font.setPointSize(9);
            msgPage->_ssidLinkCardHash[dto.groupBaseInfo.ssidGroup]->setTimeContent(QString::fromStdString(
                GetCurTime::getTimeObj()->getMsgTypeTime(curTimeStamp)),
                Qt::gray,font);
            msgPage->_ssidLinkCardHash[dto.groupBaseInfo.ssidGroup]->setSubTitle(docu.toPlainText());

            // sync with server
            g_pCommonData->setMessageContentData({msgDto});
       });
    }
    this->setLayout(_layout);
    this->setObjectName("ConversationPage");
    this->setStyleSheet("#ConversationPage{background-color: rgb(242, 242, 242);border-bottom-left-radius: 30px;border-top-right-radius: 30px;}");
}

ConversationPage::~ConversationPage()
{

}