//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_SETTINGSPAGE_H
#define SYNERGYSPOT_SETTINGSPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaScrollPage.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_SETTINGS_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_SETTINGS_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pSettingsPage SettingsPage::getInstance()

class SS_API SettingsPage : public ElaScrollPage{
public:
    static SettingsPage * getInstance();
    static void destroySettingsPage();
signals:
private:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage() override;

protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();

protected slots:
private:
    // ----------------- UI -----------------
    QWidget * _centralWidget = nullptr;

    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    // --------------- BackEnd --------------

    static SettingsPage* _settingsPage;
};

#endif//SYNERGYSPOT_SETTINGSPAGE_H
