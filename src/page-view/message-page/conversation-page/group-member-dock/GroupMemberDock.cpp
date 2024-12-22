//
// Created by FU-QAQ on 2024/12/21.
//

#include "GroupMemberDock.h"
#include "help.h"

#include "ela-widget-tools/ElaListView.h"
#include "ela-widget-tools/ElaText.h"

#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QHBoxLayout>
#include <QScrollBar>

GroupMemberDock::GroupMemberDock(QWidget *parent) : ElaScrollPage(parent) {
    auto centralWid = new QWidget(this);
    _memberList         = new ElaListView(this);
    _memberModel        = new QStandardItemModel(this);

    // list setting
    QFont memberFont;
    memberFont.setFamily("微软雅黑");
    memberFont.setPointSize(8);
    _memberList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _memberList->setModel(_memberModel);
    _memberList->setFixedHeight(110);
    _memberList->setIconSize(QSize(32,32));
    _memberList->setFont(memberFont);
    auto scrollItem = _memberList->verticalScrollBar();
    scrollItem->setSingleStep(10);

    auto * layout       = new QVBoxLayout(centralWid);
    auto * title        = new ElaText(this);

    // title font
    title->setText("  群聊成员");
    title->setAlignment(Qt::AlignLeft);
    title->setTextStyle(ElaTextType::NoStyle);
    QFont font = title->font();
    font.setFamily("微软雅黑");
    font.setBold(true);
    font.setPointSize(10);
    title->setFont(font);

    // layout
    layout->addWidget(title);
    layout->addWidget(_memberList);
    layout->setContentsMargins(0,15,0,0);
    addCentralWidget(centralWid,true,true,0);
    connect(_memberList,&ElaListView::clicked,this,[=](const QModelIndex& index) {
        auto id = _memberModel->data(index,SSIDRole).toString();
        emit sigClickedMember(id);
    });
    this->setTitleVisible(false);
    this->setContentsMargins(0,0,0,0);
}

void GroupMemberDock::addMember(const QString& avatarPath,const QString& ssid,const QString& displayName) {
    if (!avatarPath.isEmpty() && !ssid.isEmpty() && !displayName.isEmpty()) {
        auto item = new QStandardItem(displayName);
        item->setData(ssid,SSIDRole);
        item->setIcon(QIcon(avatarPath));
        _memberModel->appendRow(item);
        _memberList->update();
    }
}

