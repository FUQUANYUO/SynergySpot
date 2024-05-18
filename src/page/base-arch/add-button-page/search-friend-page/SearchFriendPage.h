//
// Created by FU-QAQ on 2024/5/5.
//

#ifndef SYNERGYSPOT_SEARCHFRIENDPAGE_H
#define SYNERGYSPOT_SEARCHFRIENDPAGE_H

#include <QMainWindow>
#include "search-form/SearchForm.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class SearchFriendPage;
}
QT_END_NAMESPACE

class SearchFriendPage : public QMainWindow {
    Q_OBJECT

public:
    explicit SearchFriendPage(QWidget *parent = nullptr);
    ~SearchFriendPage() override;
public slots:
    void UpdateSearchRes(const std::string& outdto);
private:
    Ui::SearchFriendPage *ui;

    std::vector<SearchFormMeb> res;

    std::vector<QWidget*> delVec;

    int sizeRes;
    int sizePage;
};


#endif//SYNERGYSPOT_SEARCHFRIENDPAGE_H
