//
// Created by FU-QAQ on 2024/2/9.
//

#ifndef OCHAT_ARCHPAGE_H
#define OCHAT_ARCHPAGE_H

#include <QMainWindow>
#include <QtWidgets>

#include "base-arch/msg-page/MsgPage.h"
#include "base/business-listen/BusinessListen.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ArchPage;
}
QT_END_NAMESPACE

class ArchPage : public QMainWindow {
    Q_OBJECT
protected:
    inline void closeEvent(QCloseEvent *event) override {  // 重载关闭事件
        Q_UNUSED(event);
        // 执行退出程序的操作
        emit over();
    }
public:
    explicit ArchPage(QObject * obj = nullptr,QWidget *parent = nullptr);

    BusinessListen * getBusinessObj();

    // 获取维护全部聊天页面map对象
    std::unordered_map<std::string,MsgPage*>& getMsgPageMap();

    void addDelWidget(QWidget * wid);
    ~ArchPage() override;
signals:
    void over();    // 关闭释放资源信号
private:
    std::vector<QObject*> delVec;
    Ui::ArchPage *ui;

    // 监听服务端事务
    BusinessListen * bl;
    // 维护全部的好友聊天页面
    std::unordered_map<std::string,MsgPage*> lmp;
};


#endif//OCHAT_ARCHPAGE_H
