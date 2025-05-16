//
// Created by FUQAQ on 2025/2/13.
//

#ifndef EDITINFOPAGE_H
#define EDITINFOPAGE_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>

#include "define.h"

struct UserInfo;
struct GroupInfo;
struct UserBaseInfoDTO;
struct GroupBaseInfoDTO;
class ElaCalendarPicker;
class ElaLineEdit;
class ElaToolButton;

class EditInfoPage : public QDialog {
    Q_OBJECT
public:
    explicit EditInfoPage(QWidget *parent = nullptr);
    ~EditInfoPage();
signals:
    void sigEditPageClosed();
    void sigUserAvatarChanged(const QString& localPath);
    void sigUserInfoChanged(const UserBaseInfoDTO& userInfo);

    void sigGroupAvatarChanged(const QString& ssid,const QString& localPath);
    void sigGroupInfoChanged(const QString& ssid,const GroupBaseInfoDTO& userInfo);
public slots:
    void sltSetEditPageInfo(const UserInfo& info);
    void sltSetEditPageInfo(const GroupInfo& info,UserType type);
protected:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();

    virtual void paintEvent(QPaintEvent *event);
private:
    // ----------------- UI -----------------
    // user
    QWidget              *          _userEditWidget       = nullptr;
    QPushButton          *          _avatar               = nullptr;
    QLabel               *          _name                 = nullptr;
    ElaLineEdit          *          _nameLineEdit         = nullptr;
    QLabel               *          _nameSize             = nullptr;
    QLabel               *          _personalSign         = nullptr;
    ElaLineEdit          *          _personalSignEdit     = nullptr;
    QLabel               *          _personalSignSize     = nullptr;

    QLabel               *          _sex                  = nullptr;
    QComboBox            *          _sexSelected          = nullptr;
    QLabel               *          _birthday             = nullptr;
    ElaCalendarPicker    *          _birthdaySelected     = nullptr;
    QLabel               *          _province             = nullptr;
    QComboBox            *          _provinceSelected     = nullptr;
    QLabel               *          _city                 = nullptr;
    QComboBox            *          _citySelected         = nullptr;
    QLabel               *          _district             = nullptr;
    QComboBox            *          _districtSelected     = nullptr;

    // group
    QWidget              *          _groupEditWidget      = nullptr;
    QLabel               *          _groupName            = nullptr;
    ElaLineEdit          *          _groupNameLineEdit    = nullptr;
    QLabel               *          _groupNameSize        = nullptr;
    QLabel               *          _groupProfile         = nullptr;
    ElaLineEdit          *          _groupProfileEdit     = nullptr;
    QLabel               *          _groupProfileSize     = nullptr;
    ElaToolButton        *          _groupOPManagerButton = nullptr;

    QPushButton          *          _saveBtn              = nullptr;
    QPushButton          *          _cancelBtn            = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    int                             _nameRestrictedCount     = 20;
    int                             _profileRestrictedCount  = 40;

    bool                            _isGroupEdit             = false;
    bool                            _isRevised               = false;
    QString                         _avatarPath              = "-1";
    QString                         _strUserName             = "-1";
    QString                         _strPersonalSign         = "-1";
    QString                         _strSex                  = "-1";
    time_t                          _timeBirthday            = -1;
    QString                         _strProvince             = "-1";
    QString                         _strCity                 = "-1";
    QString                         _strDistrict             = "-1";

    QString                         _strGroupName            = "-1";
    QString                         _strGroupProfile         = "-1";
    QString                         _strGroupSSID            = "-1";
    // --------------- BackEnd --------------
};



#endif //EDITINFOPAGE_H
