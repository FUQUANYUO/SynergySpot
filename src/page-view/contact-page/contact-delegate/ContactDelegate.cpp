//
// Created by FU-QAQ on 2024/12/17.
//

#include "ContactDelegate.h"

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QMenu>

#include "common-data/CommonData.h"

#include "ela-widget-tools/ElaText.h"
#include "ela-widget-tools/ElaLineEdit.h"

#include <QVBoxLayout>
#include <ela-widget-tools/ElaMenu.h>


ContactDelegate::ContactDelegate(QObject *parent)  : QStyledItemDelegate(parent) {
    connect(eTheme,&ElaTheme::themeModeChanged,this,[=](ElaThemeType::ThemeMode mode) {
        mode_ = mode;
    });

    // init add grouping dialog
    _inputGroupingNameDialog = new QDialog();
    _addGroupingTitle        = new ElaText(_inputGroupingNameDialog);
    _inputGroupingName       = new ElaLineEdit(_inputGroupingNameDialog);
    _confirmButton           = new QPushButton(_inputGroupingNameDialog);
    _cancelButton            = new QPushButton(_inputGroupingNameDialog);

    _inputGroupingNameDialog->setWindowIcon(QIcon(":/contact-page/rc-page/img/SS-default-icon.jpg"));
    _inputGroupingNameDialog->setWindowTitle("添加分组");
    _inputGroupingNameDialog->setFixedSize(400,160);
    _inputGroupingNameDialog->setModal(true);
    _addGroupingTitle->setText("添加分组");
    _addGroupingTitle->setTextPixelSize(15);
    _addGroupingTitle->setAlignment(Qt::AlignCenter);
    _addGroupingTitle->setFixedHeight(30);
    _inputGroupingName->setPlaceholderText("填写分组");
    _inputGroupingName->setFixedHeight(50);
    _confirmButton->setText("确认");
    _confirmButton->setFixedWidth(80);
    _confirmButton->setMinimumHeight(40);
    _cancelButton->setText("取消");
    _cancelButton->setFixedWidth(80);
    _cancelButton->setMinimumHeight(40);

    QVBoxLayout * mainLayout = new QVBoxLayout(_inputGroupingNameDialog);
    QHBoxLayout * buttonLayout = new QHBoxLayout;

    buttonLayout->addStretch();
    buttonLayout->addWidget(_confirmButton);
    buttonLayout->addWidget(_cancelButton);

    mainLayout->addWidget(_addGroupingTitle);
    mainLayout->addWidget(_inputGroupingName);
    mainLayout->addLayout(buttonLayout);

    _confirmButton->setEnabled(false);
    _confirmButton->setStyleSheet("color:white;background-color:rgb(0,153,255);");
    _cancelButton->setStyleSheet("color:black;background-color:rgb(235,235,235);");

    connect(_inputGroupingName, &ElaLineEdit::textChanged,this,[=]() {
        QString text = _inputGroupingName->text().trimmed();
        _confirmButton->setEnabled(!text.isEmpty());
    });

    connect(_cancelButton, &QPushButton::clicked,this,[=]() {
        _inputGroupingNameDialog->close();
    });

    connect(_confirmButton, &QPushButton::clicked,this,[=]() {
        QString text = _inputGroupingName->text().trimmed();
        emit sigAddGrouping(text);
        _inputGroupingNameDialog->close();
    });

    _inputGroupingNameDialog->setStyleSheet("border-radius: 5px");
}

ContactDelegate::~ContactDelegate() {
    delete _inputGroupingNameDialog;
    _inputGroupingNameDialog = nullptr;
}

void ContactDelegate::setRightClickMenuEnabled(bool enable) {
    _menuEnable = enable;
}

void ContactDelegate::setGroupFlag() {
    _groupFlag = true;
}

void ContactDelegate::cancelGroupFlag() {
    _groupFlag = false;
}

void ContactDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // 父节点则PASS
    if(!index.parent().isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return ;
    }

    QStyleOptionViewItem myOption = option;
    initStyleOption(&myOption, index);

    const QColor backgroundColor = ElaThemeColor(mode_,WindowBase);
    const QColor hoverColor = ElaThemeColor(mode_,BasicHover);
    const QColor selectedColor = ElaThemeColor(mode_,BasicSelectedAlpha);
    const QColor textColor = ElaThemeColor(mode_,BasicText);
    // 仅根据交互状态绘制背景
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, selectedColor);
    } else {
        painter->fillRect(option.rect, backgroundColor);
    }

    int avatarSize = 50;
    QRect avatarRect(myOption.rect.left() + 5, myOption.rect.top() + (myOption.rect.height() - avatarSize) / 2, avatarSize, avatarSize);

    // 绘制圆角头像
    QIcon avatar = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    QPixmap pixmap = avatar.pixmap(avatarSize, avatarSize);
    painter->setRenderHint(QPainter::Antialiasing); // 开启抗锯齿
    QPainterPath path;
    path.addEllipse(avatarRect);
    painter->setClipPath(path);
    painter->drawPixmap(avatarRect.topLeft(), pixmap);
    painter->setClipping(false);

    // 绘制用户名
    QString username = index.data(Qt::DisplayRole).toString();
    painter->setPen(textColor);
    QFont fontOld = painter->font();
    QFont fontForName = painter->font();
    fontForName.setPointSize(12);
    fontForName.setBold(true);
    painter->setFont(fontForName);
    QFontMetrics fm(painter->font());
    QRect usernameRect(avatarRect.right() + 10, myOption.rect.top() + 10, myOption.rect.width() - avatarRect.width() - 20, fm.height());
    painter->drawText(usernameRect, Qt::AlignLeft | Qt::AlignVCenter, username);
    painter->setFont(fontOld);
//    QString alias = "";
//    painter->setPen(Qt::black);
//    painter->drawText(usernameRect, Qt::AlignLeft | Qt::AlignVCenter, alias);

    // 绘制状态
    QString status = index.data(ContactDelegate::StatusRole).toString();
    if (status.contains("在线")) {
        const QColor onlineColor = (mode_ == ElaThemeType::Dark) ? QColor(25, 180, 100) : QColor(33, 228, 137);
        painter->setBrush(onlineColor);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setBrush(QColor(33, 228, 137));
        painter->setPen(Qt::NoPen);
        QRect circleRect(usernameRect.right() - 10, usernameRect.center().y() - 5, 10, 10);
        painter->drawEllipse(circleRect);
    }

//    QString description = "";
//    painter->setPen(Qt::black);
//    QRect descriptionRect(myOption.rect.left() + 5, myOption.rect.bottom() - fm.height() - 5, myOption.rect.width() - 10, fm.height());
//    painter->drawText(descriptionRect, Qt::AlignRight | Qt::AlignBottom, description);
}


QSize ContactDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if(!index.parent().isValid()){
        return QStyledItemDelegate::sizeHint(option,index);
    }
    QFontMetrics fm(option.font);
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(70); // 设置固定高度
    return size;
}

bool ContactDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonPress && _menuEnable)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton)
        {
            // 非分组节点
            if (!model->hasChildren(index))
            {
                // 群组
                if (_groupFlag) {
                    // is user who create group
                    QString groupSSID = model->data(index,CustomRoles::SSIDRole).toString();
                    QString createSSID = g_pCommonData->getGroupBaseInfoBySSID(groupSSID).createSSID;
                    ElaMenu menu;
                    if (createSSID == g_pCommonData->getCurUserInfo().ssid) {
                        QAction *transferGroupAction = menu.addAction("转让群聊");
                        connect(transferGroupAction, &QAction::triggered, [=]() {
                            emit sigTransferGroup(model->data(index,CustomRoles::SSIDRole).toString());
                        });
                    }else {
                        QAction *removeFriendAction = menu.addAction("退出群聊");
                        connect(removeFriendAction, &QAction::triggered, [=]() {
                            emit sigLeaveGroup(model->data(index,CustomRoles::SSIDRole).toString());
                        });
                    }
                    menu.exec(mouseEvent->globalPosition().toPoint());
                }else {
                    ElaMenu menu;
                    QAction *removeFriendAction = menu.addAction("删除好友");
                    QAction *removeGroupingAction = menu.addAction("删除分组");
                    connect(removeFriendAction, &QAction::triggered, [=]() {
                        emit sigRemoveFriend(model->data(index,CustomRoles::SSIDRole).toString());
                    });
                    connect(removeGroupingAction, &QAction::triggered, [=]() {
                        emit sigRemoveGrouping(model->data(index).toString());
                    });
                    menu.exec(mouseEvent->globalPosition().toPoint());
                }
            }
            // 分组管理
            else {
                if (!_groupFlag) {
                    ElaMenu menu;
                    QAction *addGroupingAction = menu.addAction("添加分组");
                    QAction *removeGroupingAction = menu.addAction("删除分组");

                    connect(addGroupingAction, &QAction::triggered, [=]() {
                        _inputGroupingNameDialog->show();
                        _inputGroupingNameDialog->exec();
                    });
                    connect(removeGroupingAction, &QAction::triggered, [=]() {
                        QString groupingName = model->data(index).toString();
                        emit sigRemoveGrouping(model->data(index).toString());
                    });
                    menu.exec(mouseEvent->globalPosition().toPoint());
                }
            }
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
