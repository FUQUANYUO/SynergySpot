//
// Created by FU-QAQ on 2025/3/22.
//

#include "LLMAppPage.h"

#include "../../core/common-data/CommonData.h"

#include <QHBoxLayout>
#include <QtWebEngineWidgets/QWebEngineView>
#include <mutex>

LLMAppPage * LLMAppPage::_instance;
static std::mutex m;

LLMAppPage *LLMAppPage::getInstance() {
    if (_instance == nullptr) {
        m.lock();
        if (_instance == nullptr) {
            _instance = new LLMAppPage;
        }
        m.unlock();
    }
    return _instance;
}

void LLMAppPage::destroyInstance() {
    if (_instance != nullptr) {
        m.lock();
        if (_instance != nullptr) {
            delete _instance;
        }
        m.unlock();
    }
}

LLMAppPage::LLMAppPage(QWidget *parent) : ElaWidget(parent) {
    initWindow();

    initEdgeLayout();

    initContent();

    initContent();
}

LLMAppPage::~LLMAppPage() {
}

void LLMAppPage::initWindow() {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags((window()->windowFlags()) | Qt::WindowMinimizeButtonHint | Qt::FramelessWindowHint);

    _mainLayout = new QHBoxLayout(this);
    _webView    = new QWebEngineView(this);
}

void LLMAppPage::initEdgeLayout() {
    _mainLayout->addWidget(_webView);

    this->setContentsMargins(0, 0, 0, 0);
    _mainLayout->setContentsMargins(0, 0, 0, 0);
}

void LLMAppPage::initContent() {
    _webView->load(QUrl(QString::fromStdString(g_pCommonData->getYamlNode()["open-webui-info"]["url"].as<std::string>())));
}

void LLMAppPage::initConnectFunc() {
}