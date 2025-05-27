//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_SETTINGSPAGE_H
#define SYNERGYSPOT_SETTINGSPAGE_H

#include <QMainWindow>
#include <ela-widget-tools/ElaRadioButton.h>

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

class ElaToolButton;
class ElaToggleSwitch;

class SS_API SettingsPage : public ElaScrollPage{
public:
    static SettingsPage * getInstance();
    static void destroySettingsPage();

    void setMainWindowWidget(QMainWindow *parent);

    // get global value (read only)
    QString getDownloadPath() const;

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
    QWidget                 * _centralWidget      = nullptr;
    ElaToggleSwitch         * _logSwitchButton    = nullptr;
    ElaRadioButton          * _minimumButton      = nullptr;
    ElaRadioButton          * _compactButton      = nullptr;
    ElaRadioButton          * _maximumButton      = nullptr;
    ElaRadioButton          * _autoButton         = nullptr;
    ElaText                 * _downloadPath       = nullptr;
    ElaToolButton           * _downloadPathSelect = nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    QMainWindow             * _mainWindowWidget  = nullptr;
    // --------------- BackEnd --------------

    static SettingsPage* _settingsPage;
};

#endif//SYNERGYSPOT_SETTINGSPAGE_H
