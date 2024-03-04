//
// Created by FU-QAQ on 2024/2/28.
//
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QPainter>

#include "MessageList.h"

#include "db-lite/LiteConn.h"

extern std::string CurSSID;

// Listview Model
class MessageListModel : public QStandardItemModel
{
public:
    explicit MessageListModel(QObject *parent = nullptr);
};

// delegate
class MessageListDelegate : public QStyledItemDelegate
{
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

MessageList::MessageList(QListView &lv) {
    MessageListModel *mModel = new MessageListModel(&lv);
    MessageListDelegate * mDelegate = new MessageListDelegate;
    lv.setItemDelegate(mDelegate);
    lv.setModel(mModel);

    // 选中后发送选中的消息对象的 ssid/group_ssid
    connect(&lv,&QListView::clicked,this,[=](const QModelIndex &index){
       QString selectMsgItem = mModel->data(index,Qt::DisplayRole).toString();
       LOG(selectMsgItem.toStdString())
       emit MessageList::SelectedMsgItem(selectMsgItem.toStdString());
    });
}

QSize MessageListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return {200, 60}; // 设置每个项的大小
}

void MessageListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    painter->setBrush(Qt::blue);
    painter->drawRect(option.rect);

    QString msgName = index.data(Qt::DisplayRole).toString();
    QString timeInfo = "10:00";
    QString chatContent = "小明：Hello, how are you?";

    QRect firstLineRect = option.rect.adjusted(60, 5, -5, -30);
    QRect secondLineRect = option.rect.adjusted(60, 25, -5, -5);

    painter->drawText(firstLineRect, Qt::AlignLeft | Qt::AlignTop, msgName);
    painter->drawText(firstLineRect, Qt::AlignRight | Qt::AlignTop, timeInfo);
    painter->drawText(secondLineRect, Qt::AlignLeft | Qt::AlignTop, chatContent);

}
MessageListModel::MessageListModel(QObject *parent) : QStandardItemModel(parent) {
    QList<QString> msgList = {"10000001","好友1","群组2"}; // 测试
    for (const QString &_msg : msgList) {
        QStandardItem *item = new QStandardItem(_msg);
        appendRow(item);
    }
//    const std::string& folderName = ssid;
//    // 创建用户文件夹
//    std::string createFolder = "if not exist " + folderName + " mkdir " + folderName;
//    int res = system(createFolder.c_str());
//    if(res == 0){
//        LOG("the user folder create success")
//    }else{
//        LOG("error create user folder")
//    }
//
//    // 创建sqlite数据库
//    LiteConn lcon("user_msg.db");
//    // 创建表
//    std::string createUserBaseInfoTable = R"(
//                                      CREATE TABLE user_base_info (
//                                            ssid TEXT PRIMARY NOT NULL,
//                                            name TEXT NOT NULL
//                                    );)";
//    std::string createGroupBaseInfoTable = R"(
//                                      CREATE TABLE group_base_info (
//                                            ssid_group TEXT PRIMARY NOT NULL,
//                                            name TEXT NOT NULL
//                                    );)";
//    std::string createFriendshipTable = R"(
//                                      CREATE TABLE friendship (
//                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
//                                            ssid TEXT NOT NULL,
//                                            friend_ssid TEXT NOT NULL,
//                                            ship_status INTEGER NOT NULL,
//                                            create_time DATETIME DEFAULT CURRENT_TIMESTAMP
//                                    );)";
//    std::string createGroupMemberTable = R"(
//                                       CREATE TABLE group_member_info (
//                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
//                                            ssid_group TEXT NOT NULL,
//                                            ssid_member TEXT NOT NULL,
//                                            create_time DATETIME DEFAULT CURRENT_TIMESTAMP
//                                    );)";
//    std::string createUserMsgIDTable = R"(
//                                   CREATE TABLE user_message (
//                                        id INTEGER PRIMARY KEY AUTOINCREMENT,
//                                        friendship_id TEXT,
//                                        group_member_id TEXT,
//                                        content TEXT NOT NULL,
//                                        create_time DATETIME DEFAULT CURRENT_TIMESTAMP
//                                        FOREIGN KEY (friendship_id) REFERENCES friendship(id),
//                                        FOREIGN KEY (group_member_id) REFERENCES group_member_info(id)
//                                );)";
//    if(lcon.update(createUserBaseInfoTable)){
//        LOG("success create user_base_info table")
//    }
//    if(lcon.update(createGroupBaseInfoTable)){
//        LOG("success create group_base_info table")
//    }
//    if(lcon.update(createFriendshipTable)){
//        LOG("success create friendship table")
//    }
//    if(lcon.update(createGroupMemberTable)){
//        LOG("success create group_member table")
//    }
//    if(lcon.update(createUserMsgIDTable)){
//        LOG("success create user_msg table")
//    }
//
//    // 读取数据
//    char ** resID,** resName;
//    int nrow_1 = 0,ncolumn_1 = 0,nrow_2 = 0,ncolumn_2 = 0;
//    std::string sql = "SELECT `friendship_id`,`group_member_id` FROM user_message ORDER BY create_time DESC;";
//    resID = lcon.query(sql,nrow_1,ncolumn_1);
//    for(int i = 1;i <= nrow_1;i ++ ){
//
//        if(QString(resID[i]).isEmpty()){// friendship_id
//            std::string friendship_id(resID[i]);
//            sql = "SELECT `name` FROM user_base_info WHERE ssid=(SELECT `friend_ssid` FROM friendship WHERE id=" + friendship_id + ")";
//        }else{// group_member_id
//            std::string group_member_id(resID[i]);
//            sql = "SELECT `name` FROM group_base_info WHERE ssid=(SELECT `ssid_group` FROM group_member_info WHERE id=" + group_member_id + ")";
//        }
//        resName = lcon.query(sql,nrow_2,ncolumn_2);
//        for(int j = 1;j <= nrow_2;j ++ ){
//            if(QString(resID[i]).isEmpty())
//
//        }
//    }
}
