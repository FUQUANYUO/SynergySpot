//
// Created by FUQAQ on 2025/1/4.
//

#include "SignUpPage.h"
#include "help.h"
#include "../../effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "../../plugin-manager/StrategyManager.h"


#include "ela-widget-tools/ElaPromotionView.h"
#include "ela-widget-tools/ElaPromotionCard.h"
#include "ela-widget-tools/ElaRadioButton.h"
#include "ela-widget-tools/ElaMessageBar.h"
#include "ela-widget-tools/ElaContentDialog.h"
#include "ela-widget-tools/ElaText.h"
#include "ela-widget-tools/ElaStatusBar.h"
#include "ela-widget-tools/ElaProgressBar.h"

#include <QComboBox>
#include <QLabel>
#include <QPainter>
#include <QLineEdit>
#include <QPainterPath>
#include <QPushButton>
#include <QTextBrowser>
#include <QTimer>
#include <QRandomGenerator>
#include <QVBoxLayout>

#include <mutex>
#include <regex>

SignUpPage * SignUpPage::instance = nullptr;
static std::mutex m;

SignUpPage *SignUpPage::getInstance() {
    if (instance == nullptr) {
        m.lock();
        if (instance == nullptr) {
            instance = new SignUpPage();
        }
        m.unlock();
    }
    return instance;
}
void SignUpPage::destroyInstance() {
    if (instance != nullptr) {
        m.lock();
        if (instance != nullptr) {
            instance->deleteLater();
            instance = nullptr;
        }
        m.unlock();
    }
}

void SignUpPage::initWindow() {
    setWindowTitle(tr("SignUpPage"));
    setStyleSheet("border-radius: 20px;background: rgba(255,255,255,0);");
    setWindowIcon(QIcon(":/land-page/rc-page/img/SS-default-icon.jpg"));
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setFixedSize(700,720);

    _timer                =  new QTimer(this);

    _scrollPicView        =  new ElaPromotionView(this);
    _inputName            =  new QLineEdit(this);
    _inputPassword        =  new QLineEdit(this);
    _inputRepeatPassword  =  new QLineEdit(this);
    _inputEmail           =  new QLineEdit(this);
    _inputEmailCode       =  new QLineEdit(this);
    _sendEmailCodeButton  =  new QPushButton(_inputEmailCode);
    _inputVerifyCode      =  new QLineEdit(this);
    _verifyCode           =  new QPushButton(this);
    _protocolText         =  new QTextBrowser(this);
    _acceptButton         =  new ElaRadioButton(this);
    _signUpButton         =  new QPushButton(this);
    _inputFrame           =  new QFrame(this);
    _inputLayout          =  new QVBoxLayout;
    _statusBar            =  new ElaStatusBar(this);
    _loadBar              =  new ElaProgressBar(this);
}

void SignUpPage::initEdgeLayout() {
    auto * _welcomeTitle       = new QTextBrowser(this);
    auto * _mainLayout         = new QVBoxLayout;
    auto * _protocolLayout     = new QHBoxLayout;
    auto * _verifyCodeLayout   = new QHBoxLayout;
    auto * _frameCentralLayout = new QHBoxLayout;

    _verifyCodeLayout->setContentsMargins(0,0,0,0);
    _verifyCodeLayout->setSpacing(10);
    _welcomeTitle->setText(tr(""));
    _welcomeTitle->setFixedHeight(40);
    _inputLayout->addWidget(_inputName);
    _inputLayout->addWidget(_inputPassword);
    _inputLayout->addWidget(_inputRepeatPassword);
    _verifyCodeLayout->addWidget(_inputVerifyCode);
    _verifyCodeLayout->addWidget(_verifyCode);
    _inputLayout->addWidget(_inputEmail);
    _inputLayout->addItem(_verifyCodeLayout);
    _inputLayout->addWidget(_inputEmailCode);
    _protocolLayout->addWidget(_acceptButton);
    _protocolLayout->addWidget(_protocolText);
    _inputLayout->addItem(_protocolLayout);
    _inputLayout->addWidget(_signUpButton);
    _inputLayout->setSpacing(10);
    _inputFrame->setLayout(_inputLayout);

    _frameCentralLayout->addStretch();
    _frameCentralLayout->addWidget(_inputFrame);
    _frameCentralLayout->addStretch();

    _statusBar->addWidget(_loadBar);

    _mainLayout->setContentsMargins(0,0,0,0);
    _mainLayout->setSpacing(0);
    _mainLayout->addWidget(_scrollPicView);
    _mainLayout->addWidget(_welcomeTitle);
    _mainLayout->addItem(_frameCentralLayout);
    _mainLayout->addWidget(_statusBar);

    setLayout(_mainLayout);
}

void SignUpPage::initContent() {
    // fill scrollPicView
    auto *sunriseBK = new ElaPromotionCard(this);
    auto *sunsetBK = new ElaPromotionCard(this);
    auto *brokenBK = new ElaPromotionCard(this);
    auto *towardSeaBK = new ElaPromotionCard(this);
    auto *windmillBK = new ElaPromotionCard(this);
    sunriseBK->setCardPixmap(QPixmap(":/land-page/rc-page/img/sunrise-bk.png"));
    sunsetBK->setCardPixmap(QPixmap(":/land-page/rc-page/img/sunset-bk.png"));
    brokenBK->setCardPixmap(QPixmap(":/land-page/rc-page/img/broken-bk.png"));
    towardSeaBK->setCardPixmap(QPixmap(":/land-page/rc-page/img/toward-sea-bk.png"));
    windmillBK->setCardPixmap(QPixmap(":/land-page/rc-page/img/windmill-bk.png"));

    sunriseBK->setCardTitle(tr("晨阳"));
    sunsetBK->setCardTitle(tr("余晖"));
    brokenBK->setCardTitle(tr("破碎"));
    towardSeaBK->setCardTitle(tr("旷野"));
    windmillBK->setCardTitle(tr("风车"));
    sunriseBK->setPromotionTitle(tr("粼粼晨光铺在海面~"));
    sunsetBK->setPromotionTitle(tr("余晖晕开在天际~"));
    brokenBK->setPromotionTitle(tr("无法使用的秋千~"));
    towardSeaBK->setPromotionTitle(tr("迎着无际海面的阵阵微风~"));
    windmillBK->setPromotionTitle(tr("风车和白云~"));
    sunriseBK->setTitle(tr("希望"));
    sunsetBK->setTitle(tr("美好"));
    brokenBK->setTitle(tr("惊慨"));
    towardSeaBK->setTitle(tr("自由"));
    windmillBK->setTitle(tr("欢快"));
    sunriseBK->setSubTitle(tr("于新生中诞生希望！"));
    sunsetBK->setSubTitle(tr("于终幕中美好退场！"));
    brokenBK->setSubTitle(tr("于宏大中惊慨渺小！"));
    towardSeaBK->setSubTitle(tr("于旷野中追寻人生的意义！"));
    windmillBK->setSubTitle(tr("于交融中寻找欢乐的意义！"));

    _scrollPicView->appendPromotionCard(sunriseBK);
    _scrollPicView->appendPromotionCard(sunsetBK);
    _scrollPicView->appendPromotionCard(brokenBK);
    _scrollPicView->appendPromotionCard(towardSeaBK);
    _scrollPicView->appendPromotionCard(windmillBK);
    _scrollPicView->setIsAutoScroll(true);

    _inputName->setPlaceholderText(tr("请输入昵称"));
    _inputName->setAlignment(Qt::AlignCenter);
    _inputPassword->setPlaceholderText(tr("请输入密码"));
    _inputPassword->setAlignment(Qt::AlignCenter);
    _inputRepeatPassword->setPlaceholderText(tr("请再次确认密码"));
    _inputRepeatPassword->setAlignment(Qt::AlignCenter);
    _inputEmail->setPlaceholderText(tr("请输入邮箱"));
    _inputEmail->setAlignment(Qt::AlignCenter);
    _inputEmailCode->setPlaceholderText(tr("请输入邮箱核验码"));
    _inputEmailCode->setAlignment(Qt::AlignCenter);
    _inputVerifyCode->setPlaceholderText(tr("请输入图文验证码"));
    _inputVerifyCode->setAlignment(Qt::AlignCenter);
    _protocolText->setOpenExternalLinks(false);
    _protocolText->setHtml(R"(
        <style>
            body {font-size: 8px;align-items: center; }
            a {color: blue;text-decoration:none;}
        </style>
        已阅读并同意<a href=service_agreement>服务协议</a>和<a href=privacy_policy>隐私保护指引</a>
    )");
    _protocolText->setFixedSize(235, 27);
    _protocolText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _verifyCode->setStyleSheet(R"(
        QPushButton {
            border-radius: 10px;
        }
    )");

    QString inputLineStyle = R"(
        QLineEdit {
            background: rgba(255, 255, 255, 100);
            border-radius: 10px;
            border: none;
            font-size: 16px;
        }
    )";
    int height = 35;
    int width = 260;
    _inputName->setFixedSize(width,height);
    _inputName->setStyleSheet(inputLineStyle);
    _inputPassword->setFixedSize(width,height);
    _inputPassword->setStyleSheet(inputLineStyle);
    _inputPassword->setEchoMode(QLineEdit::Password);
    _inputRepeatPassword->setFixedSize(width,height);
    _inputRepeatPassword->setStyleSheet(inputLineStyle);
    _inputRepeatPassword->setEchoMode(QLineEdit::Password);
    _inputEmail->setFixedSize(width,height);
    _inputEmail->setStyleSheet(inputLineStyle);
    _inputEmailCode->setFixedSize(width,height);
    _inputEmailCode->setStyleSheet(inputLineStyle);
    _inputVerifyCode->setFixedSize(width - 100,height);
    _inputVerifyCode->setStyleSheet(inputLineStyle);
    _verifyCode->setFixedSize(90,height);

    _signUpButton->setText("注册账号");
    _signUpButton->setStyleSheet("border: none;background: rgb(0,102,204);color: rgb(255,255,255);border-radius: 10px;");
    _signUpButton->setFixedSize(260,40);

    _sendEmailCodeButton->setFixedSize(QSize(40,20));
    _sendEmailCodeButton->setText(tr("发送"));
    _sendEmailCodeButton->setStyleSheet(R"(
        QPushButton {
            border-left: 2px solid gray;
            color: blue;
            padding-left: 5px;
        }
    )");

    _sendEmailCodeButton->setGeometry(_inputEmailCode->pos().x()+210,_inputPassword->pos().y() + 7,50,30);

    _statusBar->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    _statusBar->setFixedHeight(20);
    _loadBar->setMinimum(0);
    _loadBar->setMaximum(0);
    _loadBar->setFixedHeight(10);
    _loadBar->setMinimumWidth(this->width());
    _loadBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _loadBar->hide();

    generateNumberVerifyCode();

    setAcrylicForBKMaterial(true);
}

void SignUpPage::initConnectFunc() {
    // update the bk frame
    connect(_timer, &QTimer::timeout, this, &SignUpPage::sltUpdateFrame);

    // send sign up user data to end back
    connect(_signUpButton,&QPushButton::clicked,this,&SignUpPage::sltCheckInfo);

    // protocol text jump to the page
    connect(_protocolText,&QTextBrowser::anchorClicked,this,[=](const QUrl &url) {
        if (url.toString() == "service_agreement") {
            LOG("service test success")
        } else if (url.toString() == "privacy_policy") {
            LOG("privacy test success")
        }
    });

    // get response of sign up request
    connect(this, &SignUpPage::sigSignUpResponse, this, [=](const SignUpDataStruct & data) {
        _loadBar->hide();
    });

    // refresh code
    connect(_verifyCode,&QPushButton::clicked,this, [=]() {
        generateNumberVerifyCode();
    });

    // send email verify code
    connect(_sendEmailCodeButton, &QPushButton::clicked, this, [=]() {
        // check format of email
        do {
            std::regex emailRegex("^[a-z0-9A-Z]+[- | a-z0-9A-Z . _]+@([a-z0-9A-Z]+(-[a-z0-9A-Z]+)?\\.)+[a-z]{2,}$");

            if (!_inputEmail->text().isEmpty()) {
                // check email address format right
                if (!std::regex_match(_inputEmail->text().toStdString(),emailRegex)) {
                    ElaMessageBar::error(ElaMessageBarType::BottomLeft,"错误", "邮箱格式不正确，请再次输入！",_displayTime, this);
                    break;
                }
            }
            else {
                ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请输入邮箱！",_displayTime, this);
                break;
            }

            // verify code check
            if (!_inputVerifyCode->text().isEmpty()) {
                // check verifyCode right
                if (_inputVerifyCode->text().toLower() != _numberVerifyCode.toLower()) {
                    ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "输入的图片验证码有误!(点击图片可刷新验证码)",_displayTime, this);
                    generateNumberVerifyCode();
                    break;
                }
            }else {
                ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请输入图片验证码！",_displayTime, this);
                break;
            }

            // disable 60s
            _sendEmailCodeButton->setEnabled(false);

            QTimer *t = new QTimer(this);
            connect(t, &QTimer::timeout, this, [&]() {
                if (_disableTime <= 0) {
                    _sendEmailCodeButton->setEnabled(true);
                    _sendEmailCodeButton->setText(tr("发送"));
                }else {
                    _disableTime--;
                    _sendEmailCodeButton->setText(QString::number(_disableTime));
                }
            });
            t->start(1000);

            emit sigEmailCodeRequest(_inputEmail->text());
            _loadBar->show();
        }while (false);
    });

    // get response of email code request
    connect(this, &SignUpPage::sigEmailCodeResponse, this, [=](const QString & rightEmailCode) {
        do {
            _loadBar->hide();
            _verifyEmail = true;
            _emailVerifyCode = rightEmailCode;
        }while(false);
    });

    // bk fps
    _timer->start(64);
}


SignUpPage::SignUpPage() {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}

SignUpPage::~SignUpPage() {
}

void SignUpPage::setAcrylicForBKMaterial(bool enable) {
    _enableAcrylic = enable;
}

bool SignUpPage::generateNumberVerifyCode() {
    // 生成5位随机验证码
    const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    QString result;
    const int captchaLength = 5;
    for (int i = 0; i < captchaLength; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.size());
        result.append(chars[index]);
    }

    int width = _verifyCode->width();
    int height = _verifyCode->height();
    QImage captchaImage(width, height, QImage::Format_RGB32);
    captchaImage.fill(Qt::white);

    QPainter painter(&captchaImage);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 动态计算字符间距
    const int charSpacing = width / (result.size() + 1);
    const int baseFontSize = 12;

    // 绘制验证码文字
    for (int i = 0; i < result.size(); ++i) {
        QFont font(
            "微软雅黑",
            baseFontSize + QRandomGenerator::global()->bounded(0, 5)
        );
        font.setBold(QRandomGenerator::global()->bounded(2));
        font.setItalic(QRandomGenerator::global()->bounded(2));

        // 字符变形参数
        int rotationAngle = QRandomGenerator::global()->bounded(-15, 15); // 扩大旋转范围
        int xRandomShift = QRandomGenerator::global()->bounded(0, 5);  // 扩大水平偏移
        int yRandomShift = QRandomGenerator::global()->bounded(-5, 5);    // 增加垂直偏移范围

        // 创建临时字符图片
        QImage charImage(60, 60, QImage::Format_ARGB32_Premultiplied);
        charImage.fill(Qt::transparent);

        // 绘制原始字符
        QPainter charPainter(&charImage);
        charPainter.setPen(Qt::black);
        charPainter.setFont(font);
        charPainter.drawText(charImage.rect(), Qt::AlignCenter, QString(1, result[i]));
        charPainter.end();

        // 添加字符扭曲效果
        QTransform transform;
        transform.translate(30, 30);
        transform.rotate(rotationAngle);
        transform.shear(
            QRandomGenerator::global()->generateDouble() * 0.6 - 0.3,
            QRandomGenerator::global()->generateDouble() * 0.6 - 0.3
        );
        transform.translate(-30, -30);
        QImage transformedChar = charImage.transformed(transform, Qt::SmoothTransformation);

        // 计算绘制位置（动态间距）
        int x = charSpacing * (i+1) - transformedChar.width()/2 + xRandomShift;
        int y = (height - transformedChar.height())/2 + yRandomShift;

        // 添加颜色变化
        QColor textColor(
            QRandomGenerator::global()->bounded(150),
            QRandomGenerator::global()->bounded(150),
            QRandomGenerator::global()->bounded(150)
        );

        // 绘制处理后的字符
        painter.drawImage(x, y, transformedChar);
    }

    // 绘制密集噪声点（带透明度）
    for (int i = 0; i < 600; ++i) {
        painter.setPen(QColor(QRandomGenerator::global()->bounded(256),
                            QRandomGenerator::global()->bounded(256),
                            QRandomGenerator::global()->bounded(256),
                            QRandomGenerator::global()->bounded(50, 200))); // 半透明点
        painter.drawPoint(QRandomGenerator::global()->bounded(width),
                         QRandomGenerator::global()->bounded(height));
    }

    // 添加图像扭曲效果
    QImage warpedImage = captchaImage;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int offsetX = 5 * qSin(2 * 3.1415 * y / 60.0); // 波浪形扭曲
            int offsetY = 3 * qCos(2 * 3.1415 * x / 30.0);
            if (x + offsetX < width && x + offsetX >= 0 &&
                y + offsetY < height && y + offsetY >= 0) {
                captchaImage.setPixelColor(x, y, warpedImage.pixelColor(x + offsetX, y + offsetY));
            }
        }
    }

    QPixmap pixmap = QPixmap::fromImage(captchaImage);
    QPixmap roundedPixmap(pixmap.size());
    roundedPixmap.fill(Qt::transparent);

    QPainterPath path;
    path.addRoundedRect(QRect(0, 0, pixmap.width(), pixmap.height()), 10, 10);

    QPainter painterCircle(&roundedPixmap);
    painterCircle.setRenderHint(QPainter::Antialiasing, true);
    painterCircle.setClipPath(path);
    painterCircle.drawPixmap(0, 0, pixmap);

    _verifyCode->setIcon(roundedPixmap.scaled(width, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    _verifyCode->setIconSize(QSize(width, height));
    // 保存生成的验证码
    _numberVerifyCode = result;

    return true;
}

void SignUpPage::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    auto *curPlugin = g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME);
    if (curPlugin && std::string(curPlugin->getPluginName()) == BK_PLUGIN_NAME) {
        QImage frame = dynamic_cast<CVProVideoStrategy *>(curPlugin)->getCurrentFrame();
        if (!frame.isNull()) {
            painter.drawImage(rect(), frame);
            if (_enableAcrylic) {
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

void SignUpPage::sltUpdateFrame(){
    auto * curPlugin = g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME);
    if(curPlugin && std::string(curPlugin->getPluginName()) == BK_PLUGIN_NAME){
        dynamic_cast<CVProVideoStrategy*>(curPlugin)->updateFrame();
        update();
    }
}

void SignUpPage::sltCheckInfo(){
    // consequence check the lien edit is empty
    std::regex inputRegex("^[a-zA-Z0-9_]+$");
    std::regex emailRegex("^[a-z0-9A-Z]+[- | a-z0-9A-Z . _]+@([a-z0-9A-Z]+(-[a-z0-9A-Z]+)?\\.)+[a-z]{2,}$");
    do {
        // name check
        if (!_inputName->text().isEmpty()) {
            // defence sql inject
            if (!std::regex_match(_inputName->text().toStdString(),inputRegex) && _inputName->text().length() > 20) {
                ElaMessageBar::error(ElaMessageBarType::BottomLeft,"错误", "昵称非法输入，请检查后再次输入！",_displayTime, this);
                break;
            }
        }else {
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请输入昵称（限制10个字符内）",_displayTime, this);
            break;
        }

        // pd check
        if (!_inputPassword->text().isEmpty()) {
            if (!std::regex_match(_inputPassword->text().toStdString(),inputRegex)&& _inputPassword->text().length() > 30) {
                ElaMessageBar::error(ElaMessageBarType::BottomLeft,"错误", "密码非法输入，请检查后再次输入！",_displayTime, this);
                break;
            }
        }else {
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请输入密码（限20字符以内）",_displayTime, this);
            break;
        }

        // repeat pd check
        if (!_inputRepeatPassword->text().isEmpty()) {
            if (_inputRepeatPassword->text() != _inputPassword->text()) {
                ElaMessageBar::error(ElaMessageBarType::BottomLeft,"错误", "与前一次输入的密码不同，请再次输入！",_displayTime, this);
                break;
            }
        }else {
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请再次输入密码！",_displayTime, this);
            break;
        }

        // email format check
        if (!_inputEmail->text().isEmpty()) {
            // check email address format right
            if (!std::regex_match(_inputEmail->text().toStdString(),emailRegex)) {
                ElaMessageBar::error(ElaMessageBarType::BottomLeft,"错误", "邮箱格式不正确，请再次输入！",_displayTime, this);
                break;
            }
        }else {
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请输入邮箱！",_displayTime, this);
            break;
        }

        // only check empty and the valid check in MainLogic.cpp
        if (_inputEmailCode->text().isEmpty()) {
            ElaMessageBar::error(ElaMessageBarType::BottomRight,"错误", "请输入邮箱验证码！",_displayTime, this);
            // break;
        }
        if (!_acceptButton->isChecked()) {
            // ask for accept protocol
            auto notice = new ElaContentDialog(this);
            auto noticeContent = new ElaText(this);
            connect(notice, &ElaContentDialog::rightButtonClicked, this,[=]() {
                notice->close();
            });
            connect(notice, &ElaContentDialog::leftButtonClicked, this, [=]() {
                _acceptButton->setChecked(true);
                notice->close();
            });
            noticeContent->setText("    你尚未同意《服务协议》和《隐私指引》，同意后方可注册账号😁。");
            noticeContent->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
            noticeContent->setTextPixelSize(22);
            noticeContent->setTextStyle(ElaTextType::NoStyle);
            noticeContent->setContentsMargins(10,10,10,10);

            notice->setFixedSize(480,180);
            notice->setCentralWidget(noticeContent);
            notice->setLeftButtonText("同意");
            notice->setRightButtonText("返回");
            notice->setMiddleButtonVisible(false);
            notice->exec();
        }

        if (!_verifyEmail)
            break;
        _verifyEmail = false;
        std::string name = _inputName->text().toStdString();
        std::string email = _inputEmail->text().toStdString();
        std::string password = _inputPassword->text().toStdString();

        emit sigSignUpRequest({name,email,password, ""});
        _loadBar->show();
    }while (false);
}