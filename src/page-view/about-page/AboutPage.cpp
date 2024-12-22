//
// Created by FU-QAQ on 2024/12/11.
//

#include "AboutPage.h"

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
}

void AboutPage::initEdgeLayout() {
}

void AboutPage::initContent() {
}

void AboutPage::initConnectFunc() {
}
