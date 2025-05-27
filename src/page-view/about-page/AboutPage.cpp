//
// Created by FU-QAQ on 2024/12/11.
//

#include "AboutPage.h"

#include "ela-widget-tools/ElaImageCard.h"
#include "ela-widget-tools/ElaText.h"

#include <QVBoxLayout>
#include <mutex>

AboutPage * AboutPage::_aboutPage = nullptr;
static std::mutex m;

AboutPage *AboutPage::getInstance() {
    if(_aboutPage == nullptr){
        m.lock();
        if(_aboutPage == nullptr){
            _aboutPage = new AboutPage;
        }
        m.unlock();
    }
    return _aboutPage;
}

void AboutPage::destroyAboutPage() {
    if(_aboutPage == nullptr)
        return ;
    else{
        m.lock();
        delete _aboutPage;
        _aboutPage = nullptr;
        m.unlock();
    }
}

AboutPage::AboutPage(QWidget *parent) : ElaWidget(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initContent();

    moveToCenter();
}
AboutPage::~AboutPage() {
}

void AboutPage::initWindow() {
    setWindowTitle("关于SynergySpot");
    setWindowIcon(QIcon(":/about-page/rc-page/img/SS-default-icon.jpg"));
    this->setIsFixedSize(true);
    setWindowModality(Qt::ApplicationModal);
    setWindowButtonFlags(ElaAppBarType::CloseButtonHint);
    ElaImageCard* pixCard = new ElaImageCard(this);
    pixCard->setFixedSize(60, 60);
    pixCard->setIsPreserveAspectCrop(false);
    pixCard->setCardImage(QImage(":/about-page/rc-page/img/default-avatar-1.jpg"));

    QVBoxLayout* pixCardLayout = new QVBoxLayout();
    pixCardLayout->addWidget(pixCard);
    pixCardLayout->addStretch();

    ElaText* versionText = new ElaText("SynergySpot-V0.10", this);
    QFont versionTextFont = versionText->font();
    versionTextFont.setWeight(QFont::Bold);
    versionText->setFont(versionTextFont);
    versionText->setWordWrap(false);
    versionText->setTextPixelSize(18);

    ElaText* licenseText = new ElaText("MIT授权协议", this);
    licenseText->setWordWrap(false);
    licenseText->setTextPixelSize(14);
    ElaText* supportText = new ElaText("目前Demo版仅适用于Windows\n后续预计V0.2版本移植至Linux", this);
    supportText->setWordWrap(false);
    supportText->setTextPixelSize(14);
    ElaText* contactText = new ElaText("作者邮箱: 1299254642@qq.com", this);
    contactText->setWordWrap(false);
    contactText->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contactText->setTextPixelSize(14);

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setSpacing(15);
    textLayout->addWidget(versionText);
    textLayout->addWidget(licenseText);
    textLayout->addWidget(supportText);
    textLayout->addWidget(contactText);
    textLayout->addStretch();

    QHBoxLayout* contentLayout = new QHBoxLayout();
    contentLayout->addSpacing(30);
    contentLayout->addLayout(pixCardLayout);
    contentLayout->addSpacing(30);
    contentLayout->addLayout(textLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 25, 0, 0);
    mainLayout->addLayout(contentLayout);
}

void AboutPage::initEdgeLayout() {
}

void AboutPage::initContent() {
}

void AboutPage::initConnectFunc() {
}
