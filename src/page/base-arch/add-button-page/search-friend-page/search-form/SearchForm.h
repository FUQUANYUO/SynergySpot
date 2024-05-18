//
// Created by FU-QAQ on 2024/5/6.
//

#ifndef SYNERGYSPOT_SEARCHFORM_H
#define SYNERGYSPOT_SEARCHFORM_H

#include <QWidget>
#include "help.h"

struct SearchFormMeb{
    std::string nickname;
    std::string ssid;
};

QT_BEGIN_NAMESPACE
namespace Ui {
    class SearchForm;
}
QT_END_NAMESPACE

class SearchForm : public QWidget {
    Q_OBJECT

public:
    explicit SearchForm(QWidget *parent = nullptr);
    ~SearchForm() override;

    void setApplyBtnText(const std::string& text);

    void setSearchFormDisplay(SearchFormMeb &sfm);
private:
    Ui::SearchForm *ui;
};


#endif//SYNERGYSPOT_SEARCHFORM_H
