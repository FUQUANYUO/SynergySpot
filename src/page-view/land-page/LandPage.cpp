//
// Created by FU-QAQ on 2024/9/12.
//

#include <QMovie>
#include "ela-widget-tools/ElaApplication.h"
#include "LandPage.h"

LandPage * LandPage::_landPage = nullptr;

LandPage *LandPage::getLandPage() {
    if(_landPage == nullptr){
        _landPage = new LandPage();
    }
    return _landPage;
}

void LandPage::destroyLandPage() {
    if(_landPage == nullptr)
        return;
    else
        _landPage->deleteLater();
}


LandPage::LandPage(QWidget *parent)
    : SSUi::ArchPageWidget(parent)
{
    LandPage::initWindow();

    LandPage::initEdgeLayout();

    LandPage::initContent();

    LandPage::initConnectFunc();
}

LandPage::~LandPage() {
    destroyLandPage();
}

void LandPage::initWindow() {
    eApp->init();
    setWindowTitle("");
    setWindowButtonFlag(ElaAppBarType::MaximizeButtonHint,false);
    setWindowIcon(QIcon(":/land-page/rc-page/img/SSicon.jpg"));
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setFixedSize(300,460);
}

void LandPage::initEdgeLayout() {
}

void LandPage::initContent() {
    auto _gifBG = new QMovie(":/land-page/rc-page/img/backgroundImg.gif","gif",this);
    // set gif as background
    setGifForBackground(this, _gifBG);
    setAcrylicMaskAboveBackground(this);
    moveToCenter();
}

void LandPage::initConnectFunc() {
}
