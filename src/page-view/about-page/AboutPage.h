//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_ABOUTPAGE_H
#define SYNERGYSPOT_ABOUTPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaWidget.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_ABOUT_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_ABOUT_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pAboutPage AboutPage::getInstance()

class SS_API AboutPage : public ElaWidget{
public:
    static AboutPage * getInstance();
    static void destroyAboutPage();
signals:
private:
    explicit AboutPage(QWidget *parent = nullptr);
    ~AboutPage() override;

protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();

protected slots:
private:
    // ----------------- UI -----------------
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    // --------------- BackEnd --------------

    static AboutPage* _aboutPage;
};


#endif//SYNERGYSPOT_ABOUTPAGE_H
