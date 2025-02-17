//
// Created by FU-QAQ on 2024/9/12.
//

#ifndef SYNERGYSPOT_LANDPAGE_H
#define SYNERGYSPOT_LANDPAGE_H

#include <QOpenGLWidget>
#include "define.h"

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_LAND_PAGE_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_LAND_PAGE_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define g_pLandPage LandPage::getLandPage()

class QTimer;
class ElaRadioButton;
class ElaText;
class ElaPushButton;
class ElaImageCard;
class QTextBrowser;
class QGridLayout;
class QHBoxLayout;
class QPushButton;
class QComboBox;
class QLineEdit;
class QCheckBox;

class SS_API LandPage : public QOpenGLWidget  {
    Q_OBJECT
public:
    static LandPage * getLandPage();
    static void destroyInstance();
    // save qrc to file
    bool saveQrcToFile(const QString& qrcPath, const QString& targetFilePath);

    // clear password
    void clearPasswordInput();
signals:
    // sign in request
    void sigSignInRequest(const QString& SSID,const QString& password);

    // change cur widget in top
    void sigCurrentWidChanged(QWidget * curWid);
private:
    explicit LandPage(QWidget *parent = nullptr);
    ~LandPage() override;

protected:
    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();

    void setAcrylicForBKMaterial(bool enable = true);

    void paintEvent(QPaintEvent* event);
protected slots:
    void sltUpdateFrame();
private:
    // ----------------- UI -----------------
    QGridLayout   *  _GLayoutMain               =   nullptr;
    QHBoxLayout   *  _HLayoutForJumpURL         =   nullptr;
    QHBoxLayout   *  _HLayoutForAcceptProtocol  =   nullptr;

    ElaImageCard  *  _avatar                    =   nullptr;
    QComboBox     *  _accountComboBox           =   nullptr;
    QLineEdit     *  _inputPassword             =   nullptr;
    QCheckBox     *  _hideOrShowBtn             =   nullptr;
    ElaRadioButton*  _acceptButton              =   nullptr;
    QTextBrowser  *  _protocolText              =   nullptr;
    QPushButton   *  _signInButton              =   nullptr;
    QPushButton   *  _signUpButton              =   nullptr;
    QPushButton   *  _recoverPWButton           =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------



    // --------------- BackEnd --------------

    static LandPage* _landPage;
    bool _enableAcrylic;
    QTimer *timer;
};


#endif//SYNERGYSPOT_LANDPAGE_H
