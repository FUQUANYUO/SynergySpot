//
// Created by FUQAQ on 2025/2/13.
//

#include "EditInfoPage.h"

#include "ela-widget-tools/ElaLineEdit.h"
#include "ela-widget-tools/ElaCalendarPicker.h"
#include "ela-widget-tools/ElaPushButton.h"
#include "ela-widget-tools/ElaTheme.h"

#include "../../../core/common-data/CommonData.h"
#include "../arch-page/ArchPage.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QLabel>
#include <QComboBox>

#define SET_FRAME_STYLE(__FRAME_NAME__,__FRAME_LAYOUT__)                                                                   \
    QFrame *__FRAME_NAME__ = new QFrame(this);                                                                             \
    (__FRAME_NAME__)->setContentsMargins(0,0,0,0);                                                                         \
    (__FRAME_NAME__)->setFrameShape(QFrame::Panel);                                                                        \
    (__FRAME_NAME__)->setFrameShadow(QFrame::Sunken);                                                                      \
    (__FRAME_NAME__)->setStyleSheet("background-color: rgb(242, 242, 242); border-radius: 15px;");     \
    (__FRAME_NAME__)->setLayout(__FRAME_LAYOUT__);

EditInfoPage::EditInfoPage(QWidget *parent) : QDialog(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

EditInfoPage::~EditInfoPage() {
}

void EditInfoPage::initWindow() {
    QFont font;
    font.setFamily("微软雅黑");
    font.setPixelSize(15);

    setAttribute(Qt::WA_TranslucentBackground);
    resize(500,height());
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    _avatar = new ElaPushButton(this);

    _name = new QLabel("昵称",this);
    _name->setFont(font);
    _nameSize = new QLabel("0/20",this);
    _nameSize->setFont(font);
    _nameSize->setStyleSheet("color: gray;");
    _nameLineEdit = new ElaLineEdit(this);
    _nameLineEdit->setPlaceholderText("请输入你的昵称");

    _personalSign = new QLabel("个签",this);
    _personalSign->setFont(font);
    _personalSignSize = new QLabel("0/40",this);
    _personalSignSize->setFont(font);
    _personalSignSize->setStyleSheet("color: gray;");
    _personalSignEdit = new ElaLineEdit(this);
    _personalSignEdit->setPlaceholderText("请输入你的个性签名");

    _sex = new QLabel("性别",this);
    _sex->setFont(font);
    _sexSelected = new QComboBox(this);
    _birthday = new QLabel("生日",this);
    _birthday->setFont(font);
    _birthdaySelected = new ElaCalendarPicker(this);
    _province = new QLabel("省份",this);
    _province->setFont(font);
    _provinceSelected = new QComboBox(this);
    _city = new QLabel("地区",this);
    _city->setFont(font);
    _citySelected = new QComboBox(this);
    _district = new QLabel("街道",this);
    _district->setFont(font);
    _districtSelected = new QComboBox(this);

    _saveBtn = new QPushButton(this);
    _cancelBtn = new QPushButton(this);
}

void EditInfoPage::initEdgeLayout() {
    auto *main = new QVBoxLayout(this);
    main->setSpacing(15);
    main->setContentsMargins(10,10,10,10);
    main->setAlignment(Qt::AlignCenter);

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

    main->addLayout(avatarLayout);
    main->addWidget(nameFrame);
    main->addWidget(personalFrame);
    main->addWidget(sexFrame);
    main->addWidget(birthdayFrame);
    main->addWidget(provinceFrame);
    main->addItem(cityAndDistrictLayout);
    main->addItem(btnLayout);
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

    setFixedSize(400,600);
    _avatar->setFixedSize(80,80);
    _avatar->setBorderRadius(80);
    _avatar->setIcon(QIcon(g_pCommonData->getCurUserInfo().avatarPath));
    _avatar->setIconSize(QSize(60,60));

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

    _saveBtn->setText("保存");
    _cancelBtn->setText("取消");
    _saveBtn->setFixedSize(60,30);
    _cancelBtn->setFixedSize(60,30);
    _saveBtn->setStyleSheet("QPushButton{ background-color: #0099FF; border-radius: 5px; color: white;}");
    _cancelBtn->setStyleSheet("QPushButton{ background-color: #FFFFFF; border-radius: 5px; color: black;}");
}

void EditInfoPage::initConnectFunc() {
    connect(_cancelBtn,&QPushButton::clicked,this,&EditInfoPage::sigEditPageClosed);
    connect(_saveBtn,&QPushButton::clicked,this,[=]() {
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