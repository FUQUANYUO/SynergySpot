//
// Created by FU-QAQ on 2024/9/10.
//

#ifndef SYNERGYSPOT_ARCHPAGE_H
#define SYNERGYSPOT_ARCHPAGE_H

#include "define.h"
#include "help.h"
#include "ela-widget-tools/ElaWindow.h"
#include "ela-widget-tools/ElaWidget.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_ARCH_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_ARCH_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif


class SSMaskWidget;
class ElaContentDialog;
class ElaStatusBar;
class ElaText;
class ElaToolBar;
class ElaToolButton;
class ElaSuggestBox;
class MessagePage;
class AboutPage;
class ContactPage;
class FileManagerPage;
class SettingsPage;

#define g_pArchPage ArchPage::getInstance()

class SS_API ArchPage : public ElaWindow{
    Q_OBJECT
public:
    static ArchPage * getInstance();
    static void destroyInstance();
    void setInstanceParent(QObject * parent);

public slots:
    void sltShowMaskEffect();
    void sltHideMaskEffect();

    void sltTriggerUpdate();
private:
    explicit ArchPage(QWidget * parent = nullptr);
    ~ArchPage() override;
protected:
    void initWindow();
    void initEdgeLayout();
    void initContent();
    void initConnectFunc();

    virtual void resizeEvent(QResizeEvent * event) override;
private:
    // ----------------- UI -----------------
    ElaContentDialog * _closeDialog      = nullptr;
    ElaStatusBar     * _statusBar        = nullptr;
    ElaText          * _statusText       = nullptr;
    ElaToolBar       * _toolBar          = nullptr;
    ElaToolButton    * _addButton        = nullptr;
    ElaSuggestBox    * _searchSuggest    = nullptr;

    QAction          * _createAction     = nullptr;
    QAction          * _addAction        = nullptr;
    SSMaskWidget     * _maskWidget       = nullptr;

    int                _msgNoticeNum     = 0;
    int                _contactNoticeNum = 0;
    QString            _aboutKey         = "about";
    QString            _settingsKey      = "settings";
    QString            _fileManagerKey   = "fileManager";
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    // --------------- BackEnd --------------

    static ArchPage * _obj;
};

#endif //SYNERGYSPOT_ARCHPAGE_H
