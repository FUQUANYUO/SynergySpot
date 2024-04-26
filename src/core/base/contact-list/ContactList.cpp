//
// Created by FU-QAQ on 2024/4/2.
//
#include <algorithm>

#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QPainter>
#include <QMouseEvent>

#include "ContactList.h"
#include "base-arch/arch-page/ArchPage.h"

//-----------    protobuf    -----------//
#include "friend/FriendDTO.pb.h"

extern std::string CurSSID;

// ssid(user) : {nickname,remark}
std::map<std::string,std::pair<std::string,std::string>> sWithInfo;
// ssid(group) : {nickname,remark}
std::map<std::string,std::pair<std::string,std::string>> gsWithInfo;

// Listview Model
class ContactListModel : public QStandardItemModel
{
public:
    explicit ContactListModel(QObject *parent = nullptr,const QString& groupName = "");
    void addItem(QStandardItem * rootItem,const std::string &ssid);
private:
    std::vector<QStandardItem*> delVec;
};
void ContactListModel::addItem(QStandardItem * rootIt,const std::string &ssid) {
    auto *item = new QStandardItem(QString::fromStdString(ssid));
    item->setData(ssid.c_str(),Qt::DisplayRole);
    item->setEditable(false);

    rootIt->appendRow(item);
    dynamic_cast<QTreeView*>(parent())->update();
}

ContactListModel::ContactListModel(QObject *parent,const QString& groupName) : QStandardItemModel(parent) {
    auto * groupItem = new QStandardItem(groupName);
    groupItem->setEditable(false);

    // 目前项目需求只有两个 tab obj
    if(groupName == "好友")
        for(auto & it : sWithInfo){
            addItem(groupItem,it.first);
        }
    else if(groupName == "群组")
        for(auto & it : gsWithInfo){
            addItem(groupItem,it.first);
        }
    this->appendRow(groupItem);
}

// delegate
class ContactListDelegate : public QStyledItemDelegate
{
public:
    [[nodiscard]] QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    // 鼠标事件重载
    bool editorEvent(QEvent *event, QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index) override;
};

QSize ContactListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    return {200, 60}; // 设置每个项的大小
}
void ContactListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    // 选中变色
    QColor color(255,255,255);
    if(option.state & QStyle::State_MouseOver){
        color.setRgb(240,240,240);
    }
    painter->setBrush(color);
    painter->drawRect(option.rect);

    if(index.data(Qt::DisplayRole).toString().isEmpty()) return ;
    QString _ssid = index.data(Qt::DisplayRole).toString();
    QString msgName = "";
    auto findName = sWithInfo.find(_ssid.toStdString());
    if(findName == sWithInfo.end()){// 群信息
        std::string _remark = gsWithInfo[_ssid.toStdString()].second;
        std::string _nickname = gsWithInfo[_ssid.toStdString()].first;
        if(_remark.empty()){
            msgName = QString::fromStdString(_nickname);
        }else{
            msgName = QString::fromStdString(_remark + " (" + _nickname + ")");
        }
    }// 好友信息
    else{
        std::string _remark = sWithInfo[_ssid.toStdString()].second;
        std::string _nickname = sWithInfo[_ssid.toStdString()].first;
        if(_remark.empty()){
            msgName = QString::fromStdString(_nickname);
        }else{
            msgName = QString::fromStdString(_remark + " (" + _nickname + ")");
        }
    }

    QRect firstLineRect = option.rect.adjusted(40, 5, -5, -30);
//    QRect secondLineRect = option.rect.adjusted(60, 25, -5, -5);

    painter->setPen(QColor(0,0,0));
    painter->drawText(firstLineRect, Qt::AlignLeft | Qt::AlignTop, msgName);
//    painter->drawText(firstLineRect, Qt::AlignRight | Qt::AlignTop, xxxx);
//    painter->drawText(secondLineRect, Qt::AlignLeft | Qt::AlignTop, xxxx);
}

bool ContactListDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if(event->type() == QEvent::MouseButtonPress)
    {
        auto *mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton)
        {
            return QStyledItemDelegate::editorEvent(event, model, option, index);
        }
        else
        {
            return true; // 忽略非左键的鼠标点击事件
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        QStyleOptionViewItem opt = option;
        opt.state &= ~QStyle::State_Selected; // 清除选中状态
        return true; // 处理了鼠标释放事件
    }
    else
    {
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
}

ContactList::ContactList(QHash<QString,QTreeView*> ltv,QObject * obj) : _ltv(std::move(ltv)){
    // 发送请求对象
    BusinessListen * bl = dynamic_cast<ArchPage*>(obj)->getBusinessObj();

    // 解析联系人列表相关数据
    connect(bl,&BusinessListen::GET_CONTACTLIST,this,[&](const std::string& rawGfdto){
        SSDTO::GetFriendList_DTO gfdto;
        gfdto.ParseFromString(rawGfdto);

        if(CurSSID == gfdto.request_ssid()){
            for(int i = 0;i < gfdto.friend_infos_size();i++){
                // 提取 FriendInfo
                std::string fssid = gfdto.friend_infos(i).ssid();
                std::string fnickname = gfdto.friend_infos(i).nickname();
                std::string fremark = gfdto.friend_infos(i).remark();
                // 群
                if(gfdto.friend_infos(i).is_group()){
                    gsWithInfo.insert({fssid,{fnickname,fremark}});
                }// 好友
                else{
                    sWithInfo.insert({fssid,{fnickname,fremark}});
                }
            }
        }

        // 绘制委托样式对象
        auto * cDelegate = new ContactListDelegate;
        delVec.push_back(cDelegate);

        foreach(const auto& tvName,_ltv.keys()){// 遍历全部的分页项并设置默认值
            auto * cModel = new ContactListModel(_ltv[tvName],tvName);
            cModel->setRowCount(1);

            _ltv[tvName]->setItemDelegate(cDelegate);
            _ltv[tvName]->setModel(cModel);
            _ltv[tvName]->header()->hide();

            // 鼠标信息追踪
            _ltv[tvName]->setMouseTracking(true);
            _ltv[tvName]->viewport()->setMouseTracking(true);

            // 选中后发送选中的消息对象的 ssid/group_ssid
            connect(_ltv[tvName],&QListView::clicked,this,[=](const QModelIndex &index){
                QString selectMsgItem = cModel->data(index,Qt::DisplayRole).toString();
                LOG(selectMsgItem.toStdString())
                emit ContactList::SELECTED_CONTACTITEM(selectMsgItem.toStdString());
            });
        }
    });

    // 发送好友信息请求DTO
    SSDTO::GetFriendList_DTO dto;
    dto.set_type(SSDTO::Business_Type::GET_CONTACTLIST);
    dto.set_request_ssid(CurSSID);
    dto.set_ip("");
    std::string outGfdto;
    dto.SerializeToString(&outGfdto);

    emit bl->REQUEST_CONTACTLIST(outGfdto);
}

ContactList::~ContactList() {
    for(auto it : delVec){
        it->deleteLater();
    }
}
