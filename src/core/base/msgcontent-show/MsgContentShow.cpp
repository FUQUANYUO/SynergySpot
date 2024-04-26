//
// Created by FU-QAQ on 2024/3/13.
//
#include <QStyledItemDelegate>
#include <QStandardItemModel>
#include <QPainter>

#include "base-arch/arch-page/ArchPage.h"

#include "MsgContentShow.h"

#include "help.h"

// Listview Model
class MsgContentShowModel : public QStandardItemModel
{
public:
    explicit MsgContentShowModel(QObject *parent = nullptr);
    void addItem(QString sendStr,const QString &&role,const QString& path = "");// 添加用户发送数据
    void addItem(QString whoSend,QString recvStr,const QString &&role,const QString& path = "");// 添加服务端接收数据
};

MsgContentShowModel::MsgContentShowModel(QObject *parent) : QStandardItemModel(parent) {
}

void MsgContentShowModel::addItem(QString sendStr,const QString &&role,const QString& path) {
    auto * item = new QStandardItem(sendStr);
    item->setData(role,Qt::UserRole);
    appendRow(item);
    dynamic_cast<QListView*>(parent())->update();
}

void MsgContentShowModel::addItem(QString whoSend, QString recvStr, const QString &&role, const QString &path) {
    auto * item = new QStandardItem(recvStr);
    item->setData(role,Qt::UserRole);
    item->setData(whoSend,Qt::DecorationRole);
    appendRow(item);
    dynamic_cast<QListView*>(parent())->update();
}

// delegate
class MsgContentShowDelegate : public QStyledItemDelegate
{
public:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


QSize MsgContentShowDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    // 从模型中检索消息文本
    QString messageText = index.data(Qt::DisplayRole).toString();

    int returnCharCnt = static_cast<int>(messageText.count("\n")) + 1;

    // 计算文本的理想大小
    QFontMetrics metrics(option.font);
    int textWidth = metrics.horizontalAdvance(messageText);
    int textHeight = metrics.height() * returnCharCnt;

    // 添加一些额外的空间以适应气泡的边框和内边距
    int bubbleWidth = textWidth + 20;
    int bubbleHeight = textHeight + 20;

    return {bubbleWidth, bubbleHeight};
}

void MsgContentShowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    // 设置字体
    QFont font = painter->font();
    font.setPixelSize(14);
    painter->setFont(font);

    // 抗锯齿
    painter->setRenderHint(QPainter::Antialiasing, true);

    // 从模型中检索消息文本
    QString messageText = index.data(Qt::DisplayRole).toString();

    // 覆盖掉原项数据
    painter->setBrush(QColor(245,245,245));
    painter->setPen(Qt::NoPen);
    painter->drawRect(option.rect);

    // 计算文本的像素长度
    QFontMetrics fm(painter->font());
    int pixelsWide = fm.horizontalAdvance(messageText);
    int pixelsHigh = fm.height();

    // 将消息按照换行符分割成多行
    int returnCharCnt = static_cast<int>(messageText.count("\n")) - 1;

    int totalHeight = returnCharCnt > 0 ? returnCharCnt * pixelsHigh : 0;

    QRect bubbleRect = {};
    if(index.data(Qt::UserRole).toString() == "User"){
        // 右侧头像框
        painter->setBrush(QColor(255,255,255));
        painter->drawEllipse(option.rect.right() - 40, option.rect.top(), 40, 40);

        // 为消息气泡设置一个QRect，宽度根据messageText的像素长度动态调整
        // xp2 需要考虑到头像框的偏移量以及文本偏移
        bubbleRect = option.rect.adjusted((option.rect.width() - pixelsWide - 70), 0, -50, totalHeight);

        // 圆角气泡
        QPainterPath path;
        path.addRoundedRect(bubbleRect, 8, 8);
        painter->fillPath(path, QColor(0,153,255));

        // 在气泡内部绘制文本
        painter->setPen(Qt::white);
    }else if(index.data(Qt::UserRole).toString() == "Server"){
        // 左侧头像框
        painter->setBrush(QColor(255,255,255));
        painter->drawEllipse(option.rect.left(),option.rect.top(),40,40);

        // 为消息气泡设置一个QRect，宽度根据messageText的像素长度动态调整
        // xp2 需要考虑到头像框的偏移量以及文本偏移
        bubbleRect = option.rect.adjusted(50, 0, -(option.rect.width() - pixelsWide - 70), totalHeight);

        // 圆角气泡
        QPainterPath path;
        path.addRoundedRect(bubbleRect, 8, 8);
        painter->fillPath(path, QColor(255,255,255));

        // 在气泡内部绘制文本
        painter->setPen(Qt::black);
    }
    QRect textRect = bubbleRect.adjusted(10, 10,-10,-10);
    painter->drawText(textRect,messageText);
}


MsgContentShowModel *mModel = nullptr;
MsgContentShowDelegate *mDelegate = nullptr;
MsgContentShow::MsgContentShow(QListView *lv,QObject *obl) : _lv(lv){
    mModel = new MsgContentShowModel(_lv);
    mDelegate = new MsgContentShowDelegate();
    _lv->setItemDelegate(mDelegate);
    _lv->setModel(mModel);

    // lv 设置每个项不一样的大小
    _lv->setUniformItemSizes(false);

    // lv 间隙
    _lv->setSpacing(20);

    // lv 触发不可编辑
    _lv->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 用户发送消息的渲染
    connect(this,&MsgContentShow::PRESENT_SENDCONTENT,this,[=](const QString& sendStr){
        // 设置用户发送的消息为 User
        mModel->addItem(sendStr,"User");
    });
}
MsgContentShow::~MsgContentShow() {
    mDelegate->deleteLater();
}

void MsgContentShow::addNewItem(const std::string &ssid, const std::string &content) {
    mModel->addItem(QString::fromStdString(ssid),QString::fromStdString(content),"Server");
}
