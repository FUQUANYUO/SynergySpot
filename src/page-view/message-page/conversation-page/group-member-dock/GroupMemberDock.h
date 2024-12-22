//
// Created by FU-QAQ on 2024/12/21.
//

#ifndef GROUPMEMBERDOCK_H
#define GROUPMEMBERDOCK_H

#include "define.h"

#include <QStandardItem>
#include "ela-widget-tools/ElaScrollPage.h"

class QStandardItemModel;
class ElaListView;

class GroupMemberDock : public ElaScrollPage{
    Q_OBJECT
public:
    explicit GroupMemberDock(QWidget *parent = nullptr);
    ~GroupMemberDock() = default;

    // displayName maybe is remark/group mark/username
    void addMember(const QString& avatarPath,const QString& ssid,const QString& displayName);
signals:
    void sigClickedMember(QString ssid);
private:
    enum CustomRole {
        SSIDRole = Qt::UserRole + 1
    };

private:
    ElaListView             *   _memberList  = nullptr;
    QStandardItemModel      *   _memberModel = nullptr;
};



#endif //GROUPMEMBERDOCK_H
