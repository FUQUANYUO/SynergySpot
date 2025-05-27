//
// Created by FU-QAQ on 2024/12/17.
//

#include "ContactModel.h"

#include "../contact-delegate/ContactDelegate.h"

#include "help.h"

#include <QMimeData>
#include <qiodevice.h>

ContactModel::ContactModel(QObject *parent) : QStandardItemModel(parent) {
    addGrouping("我的好友");
}

void ContactModel::addGrouping(const QString &groupingName) {
    if (!_groupingHash.contains(groupingName)) {
        QStandardItem *parent = new QStandardItem(groupingName);
        appendRow(parent);
        _itemMap[groupingName] = QList<QPair<QStandardItem*, GroupingItem>>();
        _groupingHash.insert(groupingName, parent);
    }
}

void ContactModel::delGrouping(const QString &groupingName) {
    if (!_groupingHash.contains(groupingName) || groupingName == "我的好友")
        return;

    QStandardItem *parent = _groupingHash.value(groupingName);
    QList<QPair<QStandardItem*, GroupingItem>> items = _itemMap.value(groupingName);

    // 转移子项到默认分组
    QStandardItem *defaultParent = _groupingHash.value("我的好友");
    for (auto &pair : items) {
        parent->removeRow(pair.first->row());
        defaultParent->appendRow(pair.first);
        _itemMap["我的好友"].append(pair);
    }

    // 清理数据
    _itemMap.remove(groupingName);
    _groupingHash.remove(groupingName);
    removeRow(parent->row());
    delete parent;
}

void ContactModel::addGroupingItem(const QString &groupingName, GroupingItem item) {
    addGrouping(groupingName);

    if (QStandardItem *parent = _groupingHash.value(groupingName)) {
        QStandardItem *child = new QStandardItem(item.name);
        child->setData(QIcon(item.picPath), Qt::DecorationRole);
        child->setData(item.status, Qt::UserRole + 1);  // StatusRole
        child->setData(item.ssid, Qt::UserRole + 2);     // SSIDRole

        parent->appendRow(child);
        _itemMap[groupingName].append(qMakePair(child, item));
    }
}

void ContactModel::delGroupingItem(const QString &groupingName, const GroupingItem &item) {
    if (!_itemMap.contains(groupingName)) return;

    auto &items = _itemMap[groupingName];
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->second.ssid == item.ssid) {
            if (QStandardItem *parent = _groupingHash.value(groupingName)) {
                parent->removeRow(it->first->row());
                delete it->first;
                items.erase(it);

                if (items.isEmpty() && groupingName != "我的好友") {
                    delGrouping(groupingName);
                }
                break;
            }
        }
    }
}

QVariant ContactModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return QVariant(); // 空表头
}

Qt::ItemFlags ContactModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flags = QStandardItemModel::flags(index);

    if (index.isValid()) {
        if (index.parent().isValid()) { // 子节点可拖拽
            flags |= Qt::ItemIsDragEnabled;
        } else { // 父节点可放置
            flags |= Qt::ItemIsDropEnabled;
        }
    }
    return flags;
}

QStringList ContactModel::mimeTypes() const {
    return {"application/x-contact-data"};
}

bool ContactModel::canDropMimeData(const QMimeData *data, Qt::DropAction action,
                                  int row, int column, const QModelIndex &parent) const {
    if (!parent.isValid() || parent.parent().isValid())
        return false;

    QByteArray encoded = data->data("application/x-contact-data");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QString srcGroup, ssid;
    int srcRow;
    stream >> srcGroup >> srcRow >> ssid;

    return srcGroup != parent.data(Qt::DisplayRole).toString();
}

QMimeData* ContactModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    QByteArray encoded;
    QDataStream stream(&encoded, QIODevice::WriteOnly);

    if (!indexes.isEmpty()) {
        const QModelIndex &index = indexes.first();
        if (index.parent().isValid()) {
            stream << index.parent().data(Qt::DisplayRole).toString()
                   << index.row()
                   << index.data(Qt::UserRole + 2).toString(); // SSID
        }
    }
    mimeData->setData("application/x-contact-data", encoded);
    return mimeData;
}

bool ContactModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                              int row, int column, const QModelIndex &parent) {
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    QByteArray encoded = data->data("application/x-contact-data");
    QDataStream stream(&encoded, QIODevice::ReadOnly);
    QString srcGroup, ssid;
    int srcRow;
    stream >> srcGroup >> srcRow >> ssid;

    // 查找源项目
    auto &srcItems = _itemMap[srcGroup];
    auto it = std::find_if(srcItems.begin(), srcItems.end(),
                          [&ssid](const QPair<QStandardItem*, GroupingItem> &item) {
                              return item.second.ssid == ssid;
                          });
    if (it == srcItems.end()) return false;

    // 执行移动
    QString destGroup = parent.data(Qt::DisplayRole).toString();
    QStandardItem *destParent = _groupingHash[destGroup];
    QStandardItem *originalChild = it->first;
    GroupingItem item = it->second;

    // 创建新的子项并复制所有数据
    QStandardItem *newChild = new QStandardItem(originalChild->text());
    newChild->setData(originalChild->data(Qt::DecorationRole), Qt::DecorationRole);
    newChild->setData(originalChild->data(Qt::UserRole + 1), Qt::UserRole + 1); // StatusRole
    newChild->setData(originalChild->data(Qt::UserRole + 2), Qt::UserRole + 2); // SSIDRole

    // 从原分组移除
    int originalRow = originalChild->row();
    _groupingHash[srcGroup]->removeRow(originalRow);

    // 从源分组的映射中移除
    srcItems.erase(it);

    // 添加到目标分组
    if (row < 0 || row > destParent->rowCount()) {
        destParent->appendRow(newChild);
    } else {
        destParent->insertRow(row, newChild);
    }

    // 更新目标分组的映射
    _itemMap[destGroup].append(qMakePair(newChild, item));
    emit sigUpdateFriendshipGrouping(ssid, destGroup);
    return true;
}

void ContactModel::updateItemMapping(QStandardItem *child,
                                    const QString &oldGroup,
                                    const QString &newGroup,
                                    const GroupingItem &item) {
    _itemMap[oldGroup].removeAll(qMakePair(child, item));
    _itemMap[newGroup].append(qMakePair(child, item));
}

ContactModel * ContactModel::deepCopy(QObject *parent) const {
    ContactModel* newModel = new ContactModel(parent);
    // 复制所有分组
    for (const QString& groupName : _groupingHash.keys()) {
        newModel->addGrouping(groupName);
        QStandardItem* newParent = newModel->getParentItem(groupName);
        // 复制分组下的所有子项
        for (const auto& pair : _itemMap.value(groupName)) {
            GroupingItem item = pair.second;
            QStandardItem* newChild = new QStandardItem(item.name);
            newChild->setData(pair.first->data(Qt::DecorationRole), Qt::DecorationRole);
            newChild->setData(item.status, Qt::UserRole + 1);
            newChild->setData(item.ssid, Qt::UserRole + 2);
            newParent->appendRow(newChild);
            newModel->_itemMap[groupName].append(qMakePair(newChild, item));
        }
    }
    return newModel;
}

QString ContactModel::getGroupingName(const QModelIndex &index) const {
    return index.data(Qt::DisplayRole).toString();
}

QStandardItem* ContactModel::getParentItem(const QString &groupingName) const {
    return _groupingHash.value(groupingName);
}