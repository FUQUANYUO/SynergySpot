//
// Created by FU-QAQ on 2024/12/17.
//

#include "ContactModel.h"

#include "../contact-delegate/ContactDelegate.h"

#include "help.h"

ContactModel::ContactModel(QObject *parent) : QStandardItemModel(parent) {
}

void ContactModel::addGrouping(const QString &groupingName) {
    bool flag = (getGrouping(groupingName)==nullptr);
    if(flag){
        QStandardItem * grouping = new QStandardItem(groupingName);
        _itemMap.insert({groupingName,grouping},{});
        appendRow(grouping);
    }
}


void ContactModel::addGroupingItem(const QString &groupingName, const GroupingItem &item) {
    addGrouping(groupingName);

    QStandardItem *gItem = new QStandardItem(item.name);
    gItem->setData(QIcon(item.pic), Qt::DecorationRole);
    gItem->setData(item.status, ContactDelegate::StatusRole);
    gItem->setData(item.ssid, ContactDelegate::SSIDRole);
    QStandardItem *parentItem = getGrouping(groupingName);
    parentItem->appendRow(gItem);
}

void ContactModel::delGroupingItem(const QString &groupingName, const GroupingItem &item) {
    // 获取父分组项
    QStandardItem *parentItem = getGrouping(groupingName);
    if (!parentItem) {
        LOG_ERROR("Grouping not found:" + groupingName.toStdString());
        return;
    }

    // 遍历父项的所有子项
    for (int row = 0; row < parentItem->rowCount(); ++row) {
        QStandardItem *childItem = parentItem->child(row);
        if (!childItem) continue;

        QString childSsid = childItem->data(ContactDelegate::SSIDRole).toString();

        if (childSsid == item.ssid) {
            parentItem->removeRow(row);
            break;
        }
    }
}
QStandardItem *ContactModel::getGrouping(const QString &groupingName) {
    for(const auto &it : _itemMap.keys()){
        if(it.first == groupingName){
            return it.second;
        }
    }
    return nullptr;
}
QVariant ContactModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return QString("");
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}