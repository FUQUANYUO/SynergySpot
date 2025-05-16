//
// Created by FU-QAQ on 2024/12/11.
//

#include "SettingsPage.h"

#include "ela-widget-tools/ElaText.h"
#include "ela-widget-tools/ElaToggleSwitch.h"
#include "ela-widget-tools/ElaScrollPageArea.h"
#include "ela-widget-tools/ElaWindow.h"

#include <QHBoxLayout>
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

void SettingsPage::setMainWindowWidget(QMainWindow *parent) {
    _mainWindowWidget = parent;
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
    setTitleVisible(false);
    _centralWidget = new QWidget(this);

    ElaText* helperText = new ElaText("应用程序设置", this);
    helperText->setWordWrap(false);
    helperText->setTextPixelSize(18);

    _logSwitchButton = new ElaToggleSwitch(this);
    ElaScrollPageArea* logSwitchArea = new ElaScrollPageArea(this);
    QHBoxLayout* logSwitchLayout = new QHBoxLayout(logSwitchArea);
    ElaText* logSwitchText = new ElaText("启用日志功能", this);
    logSwitchText->setWordWrap(false);
    logSwitchText->setTextPixelSize(15);
    logSwitchLayout->addWidget(logSwitchText);
    logSwitchLayout->addStretch();
    logSwitchLayout->addWidget(_logSwitchButton);
    connect(_logSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {

    });

    _minimumButton = new ElaRadioButton("Minimum", this);
    _compactButton = new ElaRadioButton("Compact", this);
    _maximumButton = new ElaRadioButton("Maximum", this);
    _autoButton = new ElaRadioButton("Auto", this);
    _autoButton->setChecked(true);
    ElaScrollPageArea* displayModeArea = new ElaScrollPageArea(this);
    QHBoxLayout* displayModeLayout = new QHBoxLayout(displayModeArea);
    ElaText* displayModeText = new ElaText("导航栏模式选择", this);
    displayModeText->setWordWrap(false);
    displayModeText->setTextPixelSize(15);
    displayModeLayout->addWidget(displayModeText);
    displayModeLayout->addStretch();
    displayModeLayout->addWidget(_minimumButton);
    displayModeLayout->addWidget(_compactButton);
    displayModeLayout->addWidget(_maximumButton);
    displayModeLayout->addWidget(_autoButton);

    connect(_minimumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            dynamic_cast<ElaWindow*>(_mainWindowWidget)->setNavigationBarDisplayMode(ElaNavigationType::Minimal);
        }
    });
    connect(_compactButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            dynamic_cast<ElaWindow*>(_mainWindowWidget)->setNavigationBarDisplayMode(ElaNavigationType::Compact);
        }
    });
    connect(_maximumButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            dynamic_cast<ElaWindow*>(_mainWindowWidget)->setNavigationBarDisplayMode(ElaNavigationType::Maximal);
        }
    });
    connect(_autoButton, &ElaRadioButton::toggled, this, [=](bool checked) {
        if (checked)
        {
            dynamic_cast<ElaWindow*>(_mainWindowWidget)->setNavigationBarDisplayMode(ElaNavigationType::Auto);
        }
    });

    QVBoxLayout* centerLayout = new QVBoxLayout(_centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(helperText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(logSwitchArea);
    centerLayout->addWidget(displayModeArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
}

void SettingsPage::initEdgeLayout() {
    _centralWidget->setContentsMargins(0,0,0,0);
    addCentralWidget(_centralWidget);
}
void SettingsPage::initContent() {
}
void SettingsPage::initConnectFunc() {
}