//
// Created by FUQAQ on 2025/1/4.
//

#ifndef RECOVERPWPAGE_H
#define RECOVERPWPAGE_H

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

#define g_pRecoverPWPage RecoverPWPage::getInstance()

class ElaPromotionView;
class ElaRadioButton;
class ElaStatusBar;
class ElaProgressBar;
class QLabel;
class QComboBox;
class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QTextBrowser;
class QTimer;
class QFrame;

struct RecoverPWDataStruct {
    std::string ssid;
    std::string newPassword;
};

class SS_API RecoverPWPage : public QOpenGLWidget {
 Q_OBJECT
public:
    static RecoverPWPage *getInstance();
    static void destroyInstance();

    void initConnectFunc();
    void initWindow();
    void initEdgeLayout();
    void initContent();
signals:
    // request email code
    void sigEmailCodeRequest(const QString& requestSSID,const QString& emailAddr);
    // response the request for email code
    void sigEmailCodeResponse(const QString& requestSSIDByDTO,const QString& rightEmailCode);
    // request for recover password
    void sigRecoverPasswordRequest(const RecoverPWDataStruct &data);
private:
    RecoverPWPage();
    ~RecoverPWPage();
protected:
    void setAcrylicForBKMaterial(bool enable);

    // generate number verify code
    bool generateNumberVerifyCode();

    void paintEvent(QPaintEvent* event);
protected slots:
    void sltUpdateFrame();
    void sltCheckInfo();
private:
    // ----------------- UI -----------------
    ElaPromotionView     *    _scrollPicView        =   nullptr;
    QLineEdit            *    _inputAccount         =   nullptr;
    QLineEdit            *    _inputEmail           =   nullptr;
    QLineEdit            *    _inputEmailCode       =   nullptr;
    QPushButton          *    _sendEmailCodeButton  =   nullptr;
    QLineEdit            *    _inputVerifyCode      =   nullptr;
    QPushButton          *    _verifyCode           =   nullptr;
    QLineEdit            *    _inputNewPassword     =   nullptr;
    QLineEdit            *    _inputRepeatPassword  =   nullptr;

    QPushButton          *    _confirmChangedButton =   nullptr;
    ElaStatusBar         *    _statusBar            =   nullptr;
    ElaProgressBar       *    _loadBar              =   nullptr;

    QFrame               *    _inputFrame           =   nullptr;
    QVBoxLayout          *    _inputLayout          =   nullptr;
    QTimer               *    _timer                =   nullptr;
    // ----------------- UI -----------------

    // --------------- BackEnd --------------
    bool    _verifyEmail        = false;
    int     _displayTime        = 3000;         // 信息显示时间
    int     _disableTime        = 60;           // 发送 按钮禁用时间
    QString _numberVerifyCode   = "";           // 图片验证码
    QString _emailVerifyCode    = "";           // 邮箱验证码
    // --------------- BackEnd --------------

    static RecoverPWPage *instance;
    bool _enableAcrylic = false;
};

#endif //RECOVERPWPAGE_H
