//
// Created by FU-QAQ on 2024/12/11.
//

#include "SettingsPage.h"

#include "wr-config/WRConfig.h"

#include "ela-widget-tools/ElaText.h"
#include "ela-widget-tools/ElaToggleSwitch.h"
#include "ela-widget-tools/ElaScrollPageArea.h"
#include "ela-widget-tools/ElaWindow.h"
#include "ela-widget-tools/ElaToolButton.h"

#include <QFileDialog>
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

QString SettingsPage::getDownloadPath() const {
    return _downloadPath->text();
}

SettingsPage::SettingsPage(QWidget *parent) : ElaScrollPage(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initConnectFunc();
}
SettingsPage::~SettingsPage() {
    // to set data to  ini
    WRConfig::getInstance()->setDownloadPath(_downloadPath->text());
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

    _downloadPathSelect = new ElaToolButton(this);
    _downloadPath = new ElaText(this);
    ElaScrollPageArea* downloadPathArea = new ElaScrollPageArea(this);
    QHBoxLayout* downloadPathLayout = new QHBoxLayout(downloadPathArea);
    ElaText* _downloadPathText = new ElaText("下载路径",this);
    _downloadPathSelect->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _downloadPathSelect->setElaIcon(ElaIconType::FolderMagnifyingGlass);
    _downloadPathSelect->setText("更改路径");
    _downloadPathText->setWordWrap(false);
    _downloadPathText->setTextPixelSize(15);
    _downloadPath->setWordWrap(false);
    _downloadPath->setTextPixelSize(15);
    _downloadPath->setAlignment(Qt::AlignCenter);
    downloadPathLayout->addWidget(_downloadPathText);
    downloadPathLayout->addStretch();
    downloadPathLayout->addWidget(_downloadPath);
    downloadPathLayout->addStretch();
    downloadPathLayout->addWidget(_downloadPathSelect);

    QVBoxLayout* centerLayout = new QVBoxLayout(_centralWidget);
    centerLayout->addSpacing(30);
    centerLayout->addWidget(helperText);
    centerLayout->addSpacing(10);
    centerLayout->addWidget(logSwitchArea);
    centerLayout->addWidget(displayModeArea);
    centerLayout->addWidget(downloadPathArea);
    centerLayout->addStretch();
    centerLayout->setContentsMargins(0, 0, 0, 0);
}

void SettingsPage::initEdgeLayout() {
    _centralWidget->setContentsMargins(0,0,0,0);
    addCentralWidget(_centralWidget);
}

void SettingsPage::initContent() {
    if (WRConfig::getInstance()->getDownloadPath().isEmpty()) {
        _downloadPath->setText("D:\\SynergySpotDownload");
    }else {
        _downloadPath->setText(WRConfig::getInstance()->getDownloadPath());
    }
}

void SettingsPage::initConnectFunc() {
    connect(_logSwitchButton, &ElaToggleSwitch::toggled, this, [=](bool checked) {

    });
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
    connect(_downloadPathSelect, &ElaToolButton::clicked, this, [=]() {
        QString path = QFileDialog::getExistingDirectory(this, "选择下载路径",
                                                  "D://",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (path.isEmpty()) {
            return;
        }
        _downloadPath->setText(path);
    });
}