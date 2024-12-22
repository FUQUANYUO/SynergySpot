//
// Created by FU-QAQ on 2024/12/17.
//

#include "ContactModel.h"

#include "../contact-delegate/ContactDelegate.h"

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

    QStandardItem * gItem = new QStandardItem(item.name);
    gItem->setData(QIcon(item.pic),Qt::DecorationRole);
    gItem->setData(item.status,ContactDelegate::StatusRole);

    QStandardItem * parentItem = getGrouping(groupingName);
    parentItem->appendRow(gItem);
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