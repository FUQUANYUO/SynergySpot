//
// Created by FU-QAQ on 2024/9/12.
//
#include "LandPage.h"
#include "../CommonFunc.hpp"
#include "../../core/common-data/CommonData.h"
#include "../effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "../effect-component/loading-dialog/LoadingDialog.h"
#include "../effect-component/SS-mask-widget/SSMaskWidget.h"
#include "../plugin-manager/StrategyManager.h"
#include "sign-up-page/SignUpPage.h"


#include "ela-widget-tools/ElaImageCard.h"
#include "ela-widget-tools/ElaRadioButton.h"
#include "ela-widget-tools/ElaText.h"

#include <QTextBrowser>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMovie>
#include <QPainter>
#include <QPushButton>
#include <QResource>
#include <QThread>
#include <QTimer>
#include <mutex>

LandPage * LandPage::_landPage = nullptr;
static std::mutex m;

LandPage *LandPage::getLandPage() {
    if(_landPage == nullptr){
        m.lock();
        if(_landPage == nullptr)
            _landPage = new LandPage();
        m.unlock();
    }
    return _landPage;
}

void LandPage::destroyInstance() {
    if (_landPage) {
        m.lock();
        if (_landPage) {
            _landPage->deleteLater();
            _landPage = nullptr;
        }
        m.unlock();
    }
}

void LandPage::sltShowLoading() {
    sltShowMaskEffect();
    _loadingDialog->setVisible(true);
}

void LandPage::sltHideMaskEffect()  {
    _maskWidget->startMaskAnimation(0);
}

void LandPage::sltShowMaskEffect() {
    _maskWidget->setVisible(true);
    _maskWidget->raise();
    _maskWidget->setFixedSize(this->size());
    _maskWidget->startMaskAnimation(90);
}

void LandPage::sltHideLoading(){
    _loadingDialog->setVisible(false);
    sltHideMaskEffect();
}

LandPage::LandPage(QWidget *parent)
    : QOpenGLWidget(parent)
{
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();

    ComFunc::moveToCenter(this);
}

LandPage::~LandPage() {
    timer->stop();
    g_pPluginManager->releasePlugin(BK_PLUGIN_NAME);
}

void LandPage::initWindow() {
    setWindowTitle("SynergySpot \t [" + QString(SS_VERSION) + "]");
    setStyleSheet("border-radius: 20px;background: rgba(255,255,255,0);");
    setWindowIcon(QIcon(":/land-page/rc-page/img/SS-default-icon.jpg"));
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setFixedSize(320,460);

    timer = new QTimer(this);

    _avatar          = new ElaImageCard(this);
    _accountComboBox = new QComboBox(this);
    _inputPassword   = new QLineEdit(this);
    _acceptButton    = new ElaRadioButton(this);
    _protocolText    = new QTextBrowser(this);
    _signInButton    = new QPushButton(this);
    _signUpButton    = new QPushButton(this);
    _recoverPWButton = new QPushButton(this);
    _hideOrShowBtn   = new QCheckBox(_inputPassword);

    _maskWidget      = new SSMaskWidget(this);
    _loadingDialog   = new LoadingDialog(this);

    _GLayoutMain              = new QGridLayout;
    _HLayoutForJumpURL        = new QHBoxLayout;
    _HLayoutForAcceptProtocol = new QHBoxLayout;

    _maskWidget->setParent(this);
    _maskWidget->setVisible(false);
    _loadingDialog->setParent(this);
    _loadingDialog->setVisible(false);
}

void LandPage::initEdgeLayout() {
    _HLayoutForAcceptProtocol->addWidget(_acceptButton);
    _HLayoutForAcceptProtocol->addWidget(_protocolText);

    _HLayoutForJumpURL->addWidget(_signUpButton);
    _HLayoutForJumpURL->addWidget(_recoverPWButton);

    _GLayoutMain->addWidget(_avatar,0,1,2,2,Qt::AlignCenter);
    _GLayoutMain->addWidget(_accountComboBox,2,0,1,4,Qt::AlignCenter);
    _GLayoutMain->addWidget(_inputPassword,3,0,1,4,Qt::AlignCenter);
    _GLayoutMain->addLayout(_HLayoutForAcceptProtocol,4,0,1,4,Qt::AlignCenter);
    _GLayoutMain->addWidget(_signInButton,5,0,1,4,Qt::AlignCenter);
    _GLayoutMain->addLayout(_HLayoutForJumpURL,7,1,1,2,Qt::AlignCenter);

    this->setContentsMargins(0,0,0,0);
    this->setLayout(_GLayoutMain);
}

void LandPage::initContent() {
    _avatar->setCardImage(QImage(":/land-page/rc-page/img/SS-default-icon.jpg"));
    _avatar->setFixedSize(90,90);
    _avatar->setBorderRadius(20);
    _avatar->setMaximumAspectRatio(1);

    auto* _lineEditAcc = new QLineEdit(_accountComboBox);
    _lineEditAcc->setEnabled(true);
    _lineEditAcc->setPlaceholderText("请输入SSID");
    _lineEditAcc->setAlignment(Qt::AlignCenter);
    _accountComboBox->setContentsMargins(0,0,0,0);
    _accountComboBox->setLineEdit(_lineEditAcc);
    _accountComboBox->setFixedSize(260,50);
    _accountComboBox->view()->parentWidget()->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    _accountComboBox->view()->parentWidget()->setAttribute(Qt::WA_TranslucentBackground);
    _accountComboBox->setStyleSheet(R"(
        QComboBox {
            background: rgba(255, 255, 255, 100);
            border-radius: 10px;
            border: none;
            font-size: 16px;
        }
        QComboBox::drop-down {
            width: 20px;
            image: url(":/land-page/rc-page/img/drop-down.png");
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
    )");

    auto * accountList = new QListWidget(this);
    accountList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    accountList->setContentsMargins(0,0,0,0);
    accountList->setFixedWidth(240);
    accountList->setMaximumHeight(120);
    accountList->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Maximum);

    for (const auto& it : g_pCommonData->getLoginRecord(8)) {
        // init local cache for account
        auto * data = new QListWidgetItem(it.account);
        data->setTextAlignment(Qt::AlignCenter);
        data->setSizeHint(QSize(240,50));
        accountList->addItem(data);

        // init local cache for password
        _accToPasswordHash.insert(it.account, it.plainPassword);
    }
    _accountComboBox->setModel(accountList->model());
    _accountComboBox->setView(accountList);

    _inputPassword->setEnabled(true);
    _inputPassword->setPlaceholderText("请输入SS密码");
    _inputPassword->setEchoMode(QLineEdit::Password);
    _inputPassword->setAlignment(Qt::AlignCenter);
    _inputPassword->setFixedSize(260,50);
    _inputPassword->setStyleSheet(R"(
        QLineEdit {
            background: rgba(255, 255, 255, 100);
            border-radius: 10px;
            border: none;
            font-size: 16px;
            padding-right: 30px;
        }
    )");

    _hideOrShowBtn->setGeometry(_inputPassword->pos().x()+230,_inputPassword->pos().y()+15,20,20);
    _hideOrShowBtn->setStyleSheet(R"(
        QCheckBox {
            spacing: 5px;
            border: none;
            background-color: transparent;
        }
        QCheckBox::indicator {
            width: 20px;
            height: 20px;
            border: none;
            image: url(":/land-page/rc-page/img/eyes-opened.png");
        }
        QCheckBox::indicator:checked {
            image: url(":/land-page/rc-page/img/eyes-closed.png");
        }
    )");

    // TODO: jump to protocol page
    _protocolText->setOpenLinks(false);
    _protocolText->setOpenExternalLinks(false);
    _protocolText->setHtml(R"(
        <style>
            body {font-size: 10px;align-items: center; }
            a {color: blue;text-decoration:none;}
        </style>
        已阅读并同意<a href=service_agreement>服务协议</a>和<a href=privacy_policy>隐私保护指引</a>
    )");
    _protocolText->setFixedSize(235,27);
    _protocolText->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    _acceptButton->setFixedSize(20,20);

    _signInButton->setText("登录");
    _signInButton->setStyleSheet("border: none;background: rgb(0,102,204);color: rgb(255,255,255);border-radius: 10px;");
    _signInButton->setFixedSize(260,40);

    _signUpButton->setText("注册账号");
    _signUpButton->setFlat(true);
    _signUpButton->setStyleSheet("border: none;background: transparent;color: rgb(45,107,205);font-weight: bold;");
    _signUpButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    _recoverPWButton->setText("找回密码");
    _recoverPWButton->setFlat(true);
    _recoverPWButton->setStyleSheet("border:none;background:transparent;color: rgb(45,107,205);font-weight: bold;");
    _recoverPWButton->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    setAcrylicForBKMaterial(true);

    _maskWidget->setVisible(false);
    _loadingDialog->setVisible(false);
}

void LandPage::initConnectFunc() {
    // TODO: receiver Backend data change avatar
    connect(timer, &QTimer::timeout, this, &LandPage::sltUpdateFrame);
    connect(_signUpButton,&QPushButton::clicked,this,[=]() {
        g_pSignUpPage->show();
        emit sigCurrentWidChanged(g_pSignUpPage);
    });
    connect(_recoverPWButton,&QPushButton::clicked,this,[=]() {
        // emit sigCurrentWidChanged();
    });
    connect(_signInButton,&QPushButton::clicked,this,[=](){
        isFreezeSignInBtn(true);

        QString acc =  _accountComboBox->currentText();
        QString pw  =  _inputPassword->text();
        if(!acc.isEmpty() && !pw.isEmpty())
        {
            emit sigSignInRequest(acc,pw);
        }
    });
    connect(_hideOrShowBtn,&QCheckBox::stateChanged,this,[=](int state) {
        if (state) {
            _inputPassword->setEchoMode(QLineEdit::Normal);
        }else {
            _inputPassword->setEchoMode(QLineEdit::Password);
        }
    });
    connect(_protocolText,&QTextBrowser::anchorClicked,this,[=](const QUrl &url) {
        if (url.toString() == "service_agreement") {
            LOG("service test success")
        }else if (url.toString() == "privacy_policy") {
            LOG("privacy test success")
        }
    });
    connect(_accountComboBox,&QComboBox::currentIndexChanged,this,[=](int index) {
        if (_accToPasswordHash.contains(_accountComboBox->itemData(index).toString())){
            _inputPassword->setText(_accToPasswordHash[_accountComboBox->itemData(index).toString()]);
        }
    });

    timer->start(16);
}

void LandPage::sltUpdateFrame(){
    auto * curPlugin = g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME);
    if(curPlugin && std::string(curPlugin->getPluginName()) == BK_PLUGIN_NAME){
        dynamic_cast<CVProVideoStrategy*>(curPlugin)->updateFrame();
        update();
    }
}

void LandPage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    auto * curPlugin = g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME);
    if (curPlugin && std::string(curPlugin->getPluginName()) == BK_PLUGIN_NAME) {
        QImage frame = dynamic_cast<CVProVideoStrategy*>(curPlugin)->getCurrentFrame();
        if (!frame.isNull()) {
            painter.drawImage(rect(), frame);
            if(_enableAcrylic){
                painter.setRenderHint(QPainter::Antialiasing);

                // 绘制亚克力效果
                QColor acrylicColor(245, 245, 245, 120);
                painter.setBrush(acrylicColor);
                painter.setPen(Qt::NoPen);

                painter.drawRect(rect());
            }
        }
    }
}
bool LandPage::saveQrcToFile(const QString &qrcPath, const QString &targetFilePath) {
    QResource qrc(qrcPath);
    if(!qrc.isValid()){
        LOG("qrc path not found")
        return false;
    }
    QFile targetFile(targetFilePath);
    if(!targetFile.open(QIODevice::WriteOnly)){
        LOG("failed to open target write tunnel")
        return false;
    }
    QByteArray qrcData;
    qrcData.push_back(qrc.data());
    if(targetFile.write(qrcData) == -1){
        LOG("failed to write data to target file")
        return false;
    }
    targetFile.close();
    return true;
}

void LandPage::clearPasswordInput() {
    _inputPassword->clear();
}

void LandPage::isFreezeSignInBtn(bool enable) {
    _signInButton->setEnabled(!enable);
}

void LandPage::setAcrylicForBKMaterial(bool enable) {
    _enableAcrylic = enable;
}
