//
// Created by FU-QAQ on 2024/12/11.
//

#include "SettingsPage.h"

#include <mutex>

SettingsPage * SettingsPage::_settingsPage = nullptr;
static std::mutex m;

SettingsPage *SettingsPage::getInstance() {
    if(_settingsPage == nullptr){
        m.lock();
        if(_settingsPage == nullptr){
            _settingsPage = new SettingsPage();
        }
        m.unlock();
    }
    return _settingsPage;
}
void SettingsPage::destroySettingsPage() {
    if(_settingsPage == nullptr)
        return ;
    else{
        m.lock();
        delete _settingsPage;
        _settingsPage = nullptr;
        m.unlock();
    }
}
SettingsPage::SettingsPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initContent();
}
SettingsPage::~SettingsPage() {
}

void SettingsPage::initWindow() {
    _centralWidget = new QWidget(this);
}

void SettingsPage::initEdgeLayout() {
    _centralWidget->setContentsMargins(0,0,0,0);
    addCentralWidget(_centralWidget);
}
void SettingsPage::initContent() {
    _centralWidget->setWindowTitle("Settings");
}
void SettingsPage::initConnectFunc() {
}