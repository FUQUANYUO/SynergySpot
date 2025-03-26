//
// Created by FU-QAQ on 2025/3/22.
//

#ifndef LLMAPPPAGE_H
#define LLMAPPPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaWidget.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_LLMAPP_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_LLMAPP_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pLLMAppPage LLMAppPage::getInstance()

class QHBoxLayout;
class QWebEngineView;

class SS_API LLMAppPage : public ElaWidget {
    Q_OBJECT
public:
    static LLMAppPage * getInstance();
    static void destroyInstance();

private:
    explicit LLMAppPage(QWidget *parent = nullptr);
    ~LLMAppPage();

protected:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();
private:
    // ----------------- UI -----------------
    QHBoxLayout             *       _mainLayout;
    QWebEngineView          *       _webView;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    // --------------- BackEnd --------------

    static LLMAppPage* _instance;
};



#endif //LLMAPPPAGE_H
