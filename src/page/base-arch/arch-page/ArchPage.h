//
// Created by FU-QAQ on 2024/2/9.
//

#ifndef OCHAT_ARCHPAGE_H
#define OCHAT_ARCHPAGE_H

#include <QMainWindow>
#include <QtWidgets>


QT_BEGIN_NAMESPACE
namespace Ui {
    class ArchPage;
}
QT_END_NAMESPACE

class ArchPage : public QMainWindow {
    Q_OBJECT
signals:

public:
    explicit ArchPage(QWidget *parent = nullptr);
    ~ArchPage() override;
private:
    std::vector<QWidget*> delVec;
    Ui::ArchPage *ui;
};


#endif//OCHAT_ARCHPAGE_H
