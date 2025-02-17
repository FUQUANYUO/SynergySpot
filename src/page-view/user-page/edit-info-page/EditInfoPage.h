//
// Created by FUQAQ on 2025/2/13.
//

#ifndef EDITINFOPAGE_H
#define EDITINFOPAGE_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>

#include "../../effect-component/SS-mask-widget/SSMaskWidget.h"

class ElaCalendarPicker;
class ElaPushButton;
class ElaLineEdit;

class EditInfoPage : public QDialog {
    Q_OBJECT
public:
    explicit EditInfoPage(QWidget *parent = nullptr);
    ~EditInfoPage();
signals:
    void sigEditPageClosed();
protected:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();

    virtual void paintEvent(QPaintEvent *event);
private:
    // ----------------- UI -----------------
    ElaPushButton        *          _avatar              = nullptr;
    QLabel               *          _name                = nullptr;
    ElaLineEdit          *          _nameLineEdit        = nullptr;
    QLabel               *          _nameSize            = nullptr;
    QLabel               *          _personalSign        = nullptr;
    ElaLineEdit          *          _personalSignEdit    = nullptr;
    QLabel               *          _personalSignSize    = nullptr;

    QLabel               *          _sex                 = nullptr;
    QComboBox            *          _sexSelected         = nullptr;
    QLabel               *          _birthday            = nullptr;
    ElaCalendarPicker    *          _birthdaySelected    = nullptr;
    QLabel               *          _province            = nullptr;
    QComboBox            *          _provinceSelected    = nullptr;
    QLabel               *          _city                = nullptr;
    QComboBox            *          _citySelected        = nullptr;
    QLabel               *          _district            = nullptr;
    QComboBox            *          _districtSelected    = nullptr;

    QPushButton          *          _saveBtn             = nullptr;
    QPushButton          *          _cancelBtn           = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    // --------------- BackEnd --------------
};



#endif //EDITINFOPAGE_H
