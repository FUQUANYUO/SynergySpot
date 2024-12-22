//
// Created by FU-QAQ on 2024/9/12.
//
#include "LandPage.h"
#include "help.h"
#include "../CommonFunc.hpp"
#include "../effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "../plugin-manager/StrategyManager.h"


#include "ela-widget-tools/ElaImageCard.h"
#include "ela-widget-tools/ElaRadioButton.h"
#include "ela-widget-tools/ElaText.h"
#include "ela-widget-tools/ElaPushButton.h"

#include <QTimer>
#include <QMovie>
#include <mutex>
#include <QPainter>
#include <QResource>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QThread>

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

void LandPage::destroyLandPage() {
    if(_landPage == nullptr)
        return;
    else {
        m.lock();
        delete _landPage;
        _landPage = nullptr;
        m.unlock();
    }
}


LandPage::LandPage(QWidget *parent)
    : QOpenGLWidget(parent)
{
    LandPage::initWindow();

    LandPage::initEdgeLayout();

    LandPage::initContent();

    LandPage::initConnectFunc();

    ComFunc::moveToCenter(this);
}

LandPage::~LandPage() {
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
    _inputPassword   = new QComboBox(this);
    _acceptButton    = new ElaRadioButton(this);
    _protocolText    = new ElaText(this);
    _signInButton    = new QPushButton(this);
    _signUpButton    = new QPushButton(this);
    _recoverPWButton = new QPushButton(this);

    _GLayoutMain              = new QGridLayout;
    _HLayoutForJumpURL        = new QHBoxLayout;
    _HLayoutForAcceptProtocol = new QHBoxLayout;
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
    _accountComboBox->setLineEdit(_lineEditAcc);
    _accountComboBox->setFixedSize(260,50);
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
        }
        QComboBox QAbstractItemView {
            background-color: rgb(255, 255, 255);
            alignment: center;
            border-radius: 10px;
        }
        QComboBox QAbstractItemView::item {
            text-align: center;
            alignment: center;
            height: 40px;
        }
    )");

    // TODO: read local cache to fill _accountComboBox
    _accountComboBox->addItem("1266789");
    _accountComboBox->addItem("1100231");

    auto* _lineEditPW = new QLineEdit(_inputPassword);
    _lineEditPW->setEnabled(true);
    _lineEditPW->setPlaceholderText("请输入SS密码");
    _lineEditPW->setEchoMode(QLineEdit::Password);
    _lineEditPW->setAlignment(Qt::AlignCenter);
    _inputPassword->setLineEdit(_lineEditPW);
    _inputPassword->setFixedSize(260,50);
    _inputPassword->setStyleSheet(R"(
        QComboBox {
            background: rgba(255, 255, 255, 100);
            border-radius: 10px;
            border: none;
            font-size: 16px;
        }
        QComboBox::drop-down {
            width: 20px;
            border: none;
        }
    )");

    // TODO: jump to protocol page
    _protocolText->setText(u8"已阅读并同意服务协议和SS隐私保护指引");
    _protocolText->setTextPixelSize(12);
    _protocolText->setFixedSize(230,50);
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
}

void LandPage::initConnectFunc() {
    // TODO: receiver Backend data change avatar

    connect(timer, &QTimer::timeout, this, &LandPage::sltUpdateFrame);
    connect(_signUpButton,&QPushButton::clicked,this,&LandPage::sigGotoSignUpPageRequest);
    connect(_recoverPWButton,&QPushButton::clicked,this,&LandPage::sigGotoRecoverPWPageRequest);
    connect(_signInButton,&QPushButton::clicked,this,[=](){
        QString acc =  _accountComboBox->currentText();
        QString pw  =  _inputPassword->currentText();
        if(!acc.isEmpty() && !pw.isEmpty())
        {
            emit sigSignInRequest(acc,pw);
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

void LandPage::setAcrylicForBKMaterial(bool enable) {
    _enableAcrylic = enable;
}
