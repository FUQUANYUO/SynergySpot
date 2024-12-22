//
// Created by FU-QAQ on 2024/12/11.
//

#ifndef SYNERGYSPOT_MESSAGEPAGE_H
#define SYNERGYSPOT_MESSAGEPAGE_H

#include "define.h"

#include <QWidget>

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_MESSAGE_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_MESSAGE_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

class ElaScrollPage;
class ElaTabWidget;
class ElaDockWidget;
class QVBoxLayout;

#define g_pMessagePage MessagePage::getInstance()

class SS_API MessagePage : public QWidget{
    Q_OBJECT
public:
    static MessagePage * getInstance();
    static void destroyMessagePage();
private:
    explicit MessagePage(QWidget *parent = nullptr);
    ~MessagePage() override;
protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
private:
    // ----------------- UI -----------------
    ElaScrollPage * _tempMsgList;
    ElaTabWidget  * _conversionWid;
    QVBoxLayout   * _tempMsgWidLayout;
    QWidget       * _tempMsgWid;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    // --------------- BackEnd --------------
    static MessagePage* _messagePage;
};



#endif//SYNERGYSPOT_MESSAGEPAGE_H
