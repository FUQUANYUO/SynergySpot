//
// Created by FU-QAQ on 2024/2/9.
//

#ifndef OCHAT_ARCHPAGE_H
#define OCHAT_ARCHPAGE_H

#include <QMainWindow>
#include <QtWidgets>

#include "base/business-listen/BusinessListen.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ArchPage;
}
QT_END_NAMESPACE

class ArchPage : public QMainWindow {
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event) override {  // 重载关闭事件
        Q_UNUSED(event);
        // 执行退出程序的操作
        emit over();
    }
public:
    explicit ArchPage(QObject * obj = nullptr,QWidget *parent = nullptr);
    BusinessListen * getBusinessObj();
    ~ArchPage() override;
signals:
    void over();    // 关闭释放资源信号
private:
    std::vector<QObject*> delVec;
    Ui::ArchPage *ui;

    // 监听服务端事务
    BusinessListen * bl;
};


#endif//OCHAT_ARCHPAGE_H
