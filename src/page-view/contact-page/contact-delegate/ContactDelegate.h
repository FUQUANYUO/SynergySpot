//
// Created by FU-QAQ on 2024/12/17.
//

#ifndef SYNERGYSPOT_CONTACTDELEGATE_H
#define SYNERGYSPOT_CONTACTDELEGATE_H

#include <QPushButton>
#include <QStyledItemDelegate>

#include "ela-widget-tools/ElaTheme.h"

class ElaLineEdit;
class ElaText;

class ContactDelegate : public QStyledItemDelegate {
    Q_OBJECT
signals:
    void sigAddGrouping(const QString& groupingName);
    void sigRemoveGrouping(const QString& groupingName);
    void sigRemoveFriend(const QString& friendSSID);

    void sigLeaveGroup(const QString& groupSSID);
    void sigTransferGroup(const QString& friendSSID);
public:
    ContactDelegate(QObject *parent = nullptr);
    ~ContactDelegate();

    void setRightClickMenuEnabled(bool enable);

    void setGroupFlag();
    void cancelGroupFlag();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model,
                         const QStyleOptionViewItem &option,
                         const QModelIndex &index) override;

    enum CustomRoles {
        StatusRole = Qt::UserRole + 1,
        SSIDRole   = Qt::UserRole + 2
    };
private:
    // ----------------- UI -----------------
    QDialog                 * _inputGroupingNameDialog = nullptr;
    ElaText                 * _addGroupingTitle        = nullptr;
    ElaLineEdit             * _inputGroupingName       = nullptr;
    QPushButton             * _confirmButton           = nullptr;
    QPushButton             * _cancelButton            = nullptr;
    // ----------------- UI -----------------

    bool                      _menuEnable = true;
    bool                      _groupFlag  = false;
    ElaThemeType::ThemeMode mode_ = ElaThemeType::Light;
};


#endif//SYNERGYSPOT_CONTACTDELEGATE_H
