//
// Created by FU-QAQ on 2024/2/10.
//

#ifndef OCHAT_AVATARPAGE_H
#define OCHAT_AVATARPAGE_H

#include <QWidget>


QT_BEGIN_NAMESPACE
namespace Ui {
    class AvatarPage;
}
QT_END_NAMESPACE

class AvatarPage : public QWidget {
    Q_OBJECT

public:
    explicit AvatarPage(QWidget *parent = nullptr);
    ~AvatarPage() override;
private:
    Ui::AvatarPage *ui;
};


#endif//OCHAT_AVATARPAGE_H
