//
// Created by FU-QAQ on 2024/12/17.
//

#ifndef SYNERGYSPOT_CONTACTMODEL_H
#define SYNERGYSPOT_CONTACTMODEL_H

#include <QStandardItemModel>

struct GroupingItem{
    QString ssid;
    QString name;
    QString mark;
    QString details;
    QString status;
    QString picPath;

    bool operator==(const GroupingItem& other) const {
        return ssid == other.ssid
            && name == other.name
            && picPath == other.picPath;
    }
};

class ContactModel : public QStandardItemModel{
    Q_OBJECT
signals:
    // update grouping
    void sigUpdateFriendshipGrouping(QString targetSSID,QString newGroupingName);
public:
    explicit ContactModel(QObject * parent = nullptr);
    ~ContactModel() = default;

    // add grouping
    void addGrouping(const QString &groupingName);

    // del grouping
    void delGrouping(const QString &groupingName);

    // add item
    void addGroupingItem(const QString &groupingName,GroupingItem item);

    // del item
    void delGroupingItem(const QString &groupingName, const GroupingItem &item);

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QStringList mimeTypes() const override;

    bool canDropMimeData(const QMimeData *data, Qt::DropAction action,
                       int row, int column, const QModelIndex &parent) const override;

    QMimeData* mimeData(const QModelIndexList &indexes) const override;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                     int row, int column, const QModelIndex &parent) override;

    ContactModel* deepCopy(QObject* parent) const;
private:
    QString getGroupingName(const QModelIndex &index) const;

    QStandardItem* getParentItem(const QString& groupingName) const;

    void updateItemMapping(QStandardItem *child,
                            const QString &oldGroup,
                            const QString &newGroup,
                            const GroupingItem &item);
private:
    QMap<QString,QList<QPair<QStandardItem*,GroupingItem>>> _itemMap;
    QHash<QString,QStandardItem*> _groupingHash;
};

#endif//SYNERGYSPOT_CONTACTMODEL_H
