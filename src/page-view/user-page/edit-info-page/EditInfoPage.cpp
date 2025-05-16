//
// Created by FUQAQ on 2025/2/13.
//

#include "EditInfoPage.h"

#include "ela-widget-tools/ElaLineEdit.h"
#include "ela-widget-tools/ElaCalendarPicker.h"
#include "ela-widget-tools/ElaTheme.h"
#include "ela-widget-tools/ElaToolButton.h"
#include "ela-widget-tools/ElaApplication.h"

#include "../../../core/common-data/CommonData.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <QComboBox>
#include <QFileDialog>

#define SET_FRAME_STYLE(__FRAME_NAME__,__FRAME_LAYOUT__)                                                        \
    QFrame *__FRAME_NAME__ = new QFrame(this);                                                                  \
    (__FRAME_NAME__)->setContentsMargins(0,0,0,0);                                                              \
    (__FRAME_NAME__)->setFrameShape(QFrame::Panel);                                                             \
    (__FRAME_NAME__)->setFrameShadow(QFrame::Sunken);                                                           \
    (__FRAME_NAME__)->setStyleSheet("background-color: rgb(242, 242, 242); border-radius: 15px;");              \
    (__FRAME_NAME__)->setLayout(__FRAME_LAYOUT__);

EditInfoPage::EditInfoPage(QWidget *parent) : QDialog(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

EditInfoPage::~EditInfoPage() {
}

void EditInfoPage::sltSetEditPageInfo(const UserInfo &info) {
    setFixedSize(400,600);

    // hide group container
    _groupEditWidget->hide();
    _userEditWidget->show();

    adjustSize();
    _avatar->setIcon(QIcon(info._picPath.isEmpty()?(":/user-page/rc-page/img/SS-default-icon.jpg"):info._picPath));
    _nameLineEdit->setText(info._name);
    _personalSignEdit->setText(info._signContent);
    _sexSelected->setCurrentIndex(info._sex=="男生"?1:0);
    _birthdaySelected->setText(QDateTime::fromMSecsSinceEpoch(g_pCommonData->getCurUserInfo().birthDate).toString("yyyy-MM-dd"));
    _provinceSelected->setCurrentText(info._localInfo.province);
    _citySelected->setCurrentText(info._localInfo.city);
    _districtSelected->setCurrentText(info._localInfo.district);
}

void EditInfoPage::sltSetEditPageInfo(const GroupInfo &info, UserType type) {
    setFixedSize(400,450);

    if (type == Group_Member)return;
    if (type == Group_OP || type == Group_Creater) {
        _userEditWidget->hide();
        _groupEditWidget->show();

        _groupOPManagerButton->hide();
    }
    if (type == Group_Creater) {
        _groupOPManagerButton->show();
    }
    adjustSize();
    _avatar->setIcon(QIcon(info._picPath.isEmpty()?(":/user-page/rc-page/img/SS-default-icon.jpg"):info._picPath));
    _groupNameLineEdit->setText(info._name);
    _groupProfileEdit->setText(info._resume);
    _strGroupSSID = info._ssid;
}

void EditInfoPage::initWindow() {
    eApp->init();
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(15);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    _userEditWidget = new QWidget(this);
    _groupEditWidget = new QWidget(this);

    _avatar = new QPushButton(this);

    _name = new QLabel("昵称",_userEditWidget);
    _name->setFont(font);
    _nameSize = new QLabel("0/20",_userEditWidget);
    _nameSize->setFont(font);
    _nameSize->setStyleSheet("color: gray;");
    _nameLineEdit = new ElaLineEdit(_userEditWidget);
    _nameLineEdit->setPlaceholderText("请输入你的昵称");
    _nameLineEdit->setMaxLength(_nameRestrictedCount);

    _personalSign = new QLabel("个签",_userEditWidget);
    _personalSign->setFont(font);
    _personalSignSize = new QLabel("0/40",_userEditWidget);
    _personalSignSize->setFont(font);
    _personalSignSize->setStyleSheet("color: gray;");
    _personalSignEdit = new ElaLineEdit(_userEditWidget);
    _personalSignEdit->setPlaceholderText("请输入你的个性签名");
    _personalSignEdit->setMaxLength(_profileRestrictedCount);

    _sex = new QLabel("性别",_userEditWidget);
    _sex->setFont(font);
    _sexSelected = new QComboBox(_userEditWidget);
    _birthday = new QLabel("生日",_userEditWidget);
    _birthday->setFont(font);
    _birthdaySelected = new ElaCalendarPicker(_userEditWidget);
    _province = new QLabel("省份",_userEditWidget);
    _province->setFont(font);
    _provinceSelected = new QComboBox(_userEditWidget);
    _city = new QLabel("地区",_userEditWidget);
    _city->setFont(font);
    _citySelected = new QComboBox(_userEditWidget);
    _district = new QLabel("街道",_userEditWidget);
    _district->setFont(font);
    _districtSelected = new QComboBox(_userEditWidget);

    // group
    _groupName = new QLabel("群聊名称",_groupEditWidget);
    _groupName->setFont(font);
    _groupNameLineEdit = new ElaLineEdit(_groupEditWidget);
    _groupNameLineEdit->setPlaceholderText("请输入群聊的名称");
    _groupNameLineEdit->setMaxLength(_nameRestrictedCount);
    _groupNameSize = new QLabel("0/20",_groupEditWidget);
    _groupNameSize->setFont(font);
    _groupNameSize->setStyleSheet("color: gray;");
    _groupProfile = new QLabel("群聊简介",_groupEditWidget);
    _groupProfile->setFont(font);
    _groupProfileEdit = new ElaLineEdit(_groupEditWidget);
    _groupProfileEdit->setPlaceholderText("请输入群聊简介");
    _groupProfileEdit->setMaxLength(_profileRestrictedCount);
    _groupProfileSize = new QLabel("0/40",_groupEditWidget);
    _groupProfileSize->setFont(font);
    _groupProfileSize->setStyleSheet("color: gray;");
    _groupOPManagerButton = new ElaToolButton(_groupEditWidget);
    _groupOPManagerButton->setText("群聊管理员管理");
    _groupOPManagerButton->setElaIcon(ElaIconType::FlagPennant);
    _groupOPManagerButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _groupOPManagerButton->setBorderRadius(15);
    _groupOPManagerButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    _saveBtn = new QPushButton(this);
    _cancelBtn = new QPushButton(this);

    _userEditWidget->hide();
    _groupEditWidget->hide();
}

void EditInfoPage::initEdgeLayout() {
    _userEditWidget->setContentsMargins(0,0,0,0);
    _groupEditWidget->setContentsMargins(0,0,0,0);
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(10,10,10,10);
    mainLayout->setAlignment(Qt::AlignCenter);

    auto *mainUser = new QVBoxLayout(_userEditWidget);
    mainUser->setSpacing(15);
    mainUser->setContentsMargins(0,0,0,0);
    mainUser->setAlignment(Qt::AlignCenter);

    auto *mainGroup = new QVBoxLayout(_groupEditWidget);
    mainGroup->setSpacing(15);
    mainGroup->setContentsMargins(0,0,0,0);
    mainGroup->setAlignment(Qt::AlignCenter);

    auto *avatarLayout = new QHBoxLayout;
    avatarLayout->addStretch();
    avatarLayout->addWidget(_avatar);
    avatarLayout->addStretch();

    auto *nameLayout = new QHBoxLayout;
    nameLayout->setSpacing(10);
    nameLayout->addWidget(_name);
    nameLayout->addWidget(_nameLineEdit);
    nameLayout->addWidget(_nameSize);
    SET_FRAME_STYLE(nameFrame,nameLayout)

    auto *personalLayout = new QHBoxLayout;
    personalLayout->setSpacing(10);
    personalLayout->addWidget(_personalSign);
    personalLayout->addWidget(_personalSignEdit);
    personalLayout->addWidget(_personalSignSize);
    SET_FRAME_STYLE(personalFrame,personalLayout)

    auto *sexLayout = new QHBoxLayout;
    sexLayout->setSpacing(10);
    sexLayout->addWidget(_sex);
    sexLayout->addWidget(_sexSelected);
    SET_FRAME_STYLE(sexFrame,sexLayout)

    auto *birthdayLayout = new QHBoxLayout;
    birthdayLayout->setSpacing(10);
    birthdayLayout->addWidget(_birthday);
    birthdayLayout->addWidget(_birthdaySelected);
    SET_FRAME_STYLE(birthdayFrame,birthdayLayout)

    auto *provinceLayout = new QHBoxLayout;
    provinceLayout->setSpacing(10);
    provinceLayout->addWidget(_province);
    provinceLayout->addWidget(_provinceSelected);
    SET_FRAME_STYLE(provinceFrame,provinceLayout)

    auto *cityLayout = new QHBoxLayout;
    cityLayout->setSpacing(10);
    cityLayout->addWidget(_city);
    cityLayout->addWidget(_citySelected);
    SET_FRAME_STYLE(cityFrame,cityLayout)

    auto *districtLayout = new QHBoxLayout;
    districtLayout->setSpacing(10);
    districtLayout->addWidget(_district);
    districtLayout->addWidget(_districtSelected);
    SET_FRAME_STYLE(districtFrame,districtLayout)

    // group
    auto *groupNameEditLayout = new QHBoxLayout;
    groupNameEditLayout->setSpacing(10);
    groupNameEditLayout->addWidget(_groupName);
    groupNameEditLayout->addWidget(_groupNameLineEdit);
    groupNameEditLayout->addWidget(_groupNameSize);
    SET_FRAME_STYLE(groupNameEditFrame,groupNameEditLayout)

    auto *groupProfileEditLayout = new QHBoxLayout;
    groupProfileEditLayout->setSpacing(10);
    groupProfileEditLayout->addWidget(_groupProfile);
    groupProfileEditLayout->addWidget(_groupProfileEdit);
    groupProfileEditLayout->addWidget(_groupProfileSize);
    SET_FRAME_STYLE(groupProfileEditFrame,groupProfileEditLayout)

    auto *cityAndDistrictLayout = new QHBoxLayout;
    cityAndDistrictLayout->setSpacing(20);
    cityAndDistrictLayout->addWidget(cityFrame);
    cityAndDistrictLayout->addWidget(districtFrame);

    auto *btnLayout = new QHBoxLayout;
    btnLayout->setContentsMargins(10,10,10,10);
    btnLayout->setSpacing(10);
    btnLayout->addStretch();
    btnLayout->addWidget(_saveBtn);
    btnLayout->addWidget(_cancelBtn);

    mainUser->addWidget(nameFrame);
    mainUser->addWidget(personalFrame);
    mainUser->addWidget(sexFrame);
    mainUser->addWidget(birthdayFrame);
    mainUser->addWidget(provinceFrame);
    mainUser->addLayout(cityAndDistrictLayout);
    mainGroup->addWidget(groupNameEditFrame);
    mainGroup->addWidget(groupProfileEditFrame);
    mainGroup->addWidget(_groupOPManagerButton);

    mainLayout->addLayout(avatarLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(_userEditWidget);
    mainLayout->addWidget(_groupEditWidget);
    mainLayout->addStretch();
    mainLayout->addLayout(btnLayout);
}

void EditInfoPage::initContent() {
    const QString comboBoxStyle = R"(
        QComboBox {
            background: rgba(255, 255, 255, 100);
            border-radius: 10px;
            border: none;
            font-size: 16px;
        }
        QComboBox::drop-down {
            width: 20px;
            image: url(":/user-page/rc-page/img/drop-down.png");
            padding-right: 10px;
        }
        QComboBox QAbstractItemView {
            background-color: rgba(255, 255, 255, 220);
            border: none;
            font-size: 16px;
            border-radius: 10px;
        }
        QComboBox QAbstractItemView::item {
            font-size: 16px;
            height: 40px;
        }
    )";

    _avatar->setFixedSize(80,80);
    _avatar->setIconSize(QSize(60,60));
    _avatar->setStyleSheet("border-radius: 60px;");

    _name->setFixedSize(30,40);
    _nameSize->setFixedSize(40,40);
    _nameLineEdit->setFixedSize(280,40);

    _personalSign->setFixedSize(30,40);
    _personalSignSize->setFixedSize(40,40);
    _personalSignEdit->setFixedSize(280,40);

    _sex->setFixedSize(30,40);
    _sexSelected->setFixedSize(320,40);
    _sexSelected->setStyleSheet(comboBoxStyle);
    _sexSelected->addItems({"男生","女生"});
    _sexSelected->view()->parentWidget()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _sexSelected->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);
    _birthday->setFixedSize(30,40);
    _birthdaySelected->setFixedSize(320,40);
    _province->setFixedSize(30,40);
    _provinceSelected->setFixedSize(320,40);
    _provinceSelected->setStyleSheet(comboBoxStyle);
    _provinceSelected->view()->parentWidget()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _provinceSelected->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);
    _city->setFixedSize(30,40);
    _citySelected->setFixedSize(130,40);
    _citySelected->setStyleSheet(comboBoxStyle);
    _citySelected->view()->parentWidget()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _citySelected->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);
    _district->setFixedSize(30,40);
    _districtSelected->setFixedSize(130,40);
    _districtSelected->setStyleSheet(comboBoxStyle);
    _districtSelected->view()->parentWidget()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _districtSelected->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);

    // group
    _groupName->setFixedSize(60,40);
    _groupNameSize->setFixedSize(40,40);
    _groupNameLineEdit->setFixedSize(250,40);

    _groupProfile->setFixedSize(30,40);
    _groupProfileSize->setFixedSize(40,40);
    _groupProfileEdit->setFixedSize(280,40);

    _groupOPManagerButton->setFixedHeight(60);

    _saveBtn->setText("保存");
    _cancelBtn->setText("取消");
    _saveBtn->setFixedSize(60,30);
    _cancelBtn->setFixedSize(60,30);
    _saveBtn->setStyleSheet("QPushButton{ background-color: #0099FF; border-radius: 5px; color: white;}");
    _cancelBtn->setStyleSheet("QPushButton{ background-color: #FFFFFF; border-radius: 5px; color: black;}");
}

void EditInfoPage::initConnectFunc() {
    // line edit to show in label
    connect(_nameLineEdit,&ElaLineEdit::textChanged,this,[=](const QString& str) {
        _isRevised = true;
        int curCount = str.length();
        _nameSize->setText(QString::number(curCount) + "/20");
        _strUserName = str;
    });
    connect(_personalSignEdit,&ElaLineEdit::textChanged,this,[=](const QString &str) {
        _isRevised = true;
        int curCount = str.length();
        _personalSignSize->setText(QString::number(curCount) + "/40");
        _strPersonalSign = str;
    });
    connect(_groupNameLineEdit,&ElaLineEdit::textChanged,this,[=](const QString& str) {
        _isRevised = true;
        int curCount = str.length();
        _groupNameSize->setText(QString::number(curCount) + "/20");
        _strGroupName = str;
    });
    connect(_groupProfileEdit,&ElaLineEdit::textChanged,this,[=](const QString& str) {
        _isRevised = true;
        int curCount = str.length();
        _groupProfileSize->setText(QString::number(curCount) + "/40");
        _strGroupProfile = str;
    });

    connect(_sexSelected, &QComboBox::currentIndexChanged,this,[=](int index) {
        _isRevised = true;
        _strSex = _sexSelected->currentText();
    });

    connect(_birthdaySelected, &ElaCalendarPicker::selectedDateChanged,this,[=](QDate date) {
        _isRevised = true;
        _timeBirthday = QDateTime(date,QTime(0,0,0)).toMSecsSinceEpoch();
    });

    connect(_provinceSelected,&QComboBox::currentIndexChanged,this,[=](int index) {
        _isRevised = true;
        _strProvince = _provinceSelected->currentText();
    });

    connect(_citySelected,&QComboBox::currentIndexChanged,this,[=](int index) {
        _isRevised = true;
        _strCity = _citySelected->currentText();
    });

    connect(_districtSelected,&QComboBox::currentIndexChanged,this,[=](int index) {
        _isRevised = true;
        _strDistrict = _districtSelected->currentText();
    });

    connect(_avatar,&QPushButton::clicked,[=]() {
        QString filePath = QFileDialog::getOpenFileName(
            nullptr,
            "选择文件",
            QDir::homePath(),
            "所有文件 (*);;"
        );
        if (filePath != nullptr) {
            _isRevised = true;
            _avatar->setIcon(QIcon(filePath));
            _avatarPath = filePath;
        }
    });
    connect(_cancelBtn,&QPushButton::clicked,this,&EditInfoPage::sigEditPageClosed);
    connect(_saveBtn,&QPushButton::clicked,this,[=]() {
        if (!_isGroupEdit) {
            UserBaseInfoDTO userInfo;
            if (_strUserName != "-1")
                userInfo.username = _strUserName;
            if (_avatarPath != "-1")
                userInfo.avatarPath = _avatarPath;
            if (_strPersonalSign != "-1")
                userInfo.personalSign = _strPersonalSign;
            if (_timeBirthday != -1)
                userInfo.birthDate = _timeBirthday;
            if (_strSex != "-1")
                userInfo.sex = _strSex;
            if (_avatarPath != "-1") {
                userInfo.avatarPath = _avatarPath;
                emit sigUserAvatarChanged(_avatarPath);
            }
            if (_isRevised) {
                emit sigUserInfoChanged(userInfo);
            }
        }else {
            GroupBaseInfoDTO groupInfo;
            if (_strGroupName != "-1")
                groupInfo.groupName = _strGroupName;
            if (_strGroupProfile != "-1")
                groupInfo.profile = _strGroupProfile;
            if (_avatarPath != "-1") {
                groupInfo.avatarPath = _avatarPath;
                emit sigGroupAvatarChanged(_strGroupSSID,_avatarPath);
            }
            if (_isRevised) {
                emit sigGroupInfoChanged(_strGroupSSID,groupInfo);
            }
        }
        emit sigEditPageClosed();
    });
}

void EditInfoPage::paintEvent(QPaintEvent *event) {
    // 启用抗锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    // 清空背景
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::transparent);
    painter.drawRect(rect());

    // 绘制主背景
    painter.setBrush(ElaThemeColor(eTheme->getThemeMode(), DialogBase));
    painter.drawRoundedRect(rect(), 20, 20);

    // 绘制按钮栏背景
    painter.setBrush(ElaThemeColor(eTheme->getThemeMode(), DialogLayoutArea));
    painter.drawRoundedRect(QRectF(0, height() - 60, width(), 60), 8, 8);

    QDialog::paintEvent(event);
}