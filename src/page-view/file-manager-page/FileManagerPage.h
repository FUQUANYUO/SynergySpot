//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_FILEMANAGERPAGE_H
#define SYNERGYSPOT_FILEMANAGERPAGE_H

#include "define.h"
#include "ela-widget-tools/ElaScrollPage.h"
#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_FILE_MANAGER_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_FILE_MANAGER_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pFileManagerPage FileManagerPage::getInstance()

class SS_API FileManagerPage : public ElaScrollPage{
public:
    static FileManagerPage * getInstance();
    static void destroyFileManagerPage();
signals:
private:
    explicit FileManagerPage(QWidget *parent = nullptr);
    ~FileManagerPage() override;

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

    static FileManagerPage* _fileManagerPage;
};

#endif//SYNERGYSPOT_FILEMANAGERPAGE_H
