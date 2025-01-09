//
// Created by FU-QAQ on 2024/12/10.
//

#include "MainLogic.h"

#include "core/base/client-request-handler/ClientRequestHandler.h"

#include "land-page/LandPage.h"
#include "land-page/sign-up-page/SignUpPage.h"
#include "land-page/recover-pw-page/RecoverPWPage.h"
#include "plugin-manager/StrategyManager.h"
#include "effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "arch-page/ArchPage.h"

#include "ela-widget-tools/ElaApplication.h"
#include "ela-widget-tools/ElaMessageBar.h"

//------------ begin protobuf ------------//
#include "email/EmailVerifyCode.pb.h"
//------------ end protobuf   ------------//

//------------   begin core   ------------//
#include "safety/email-check/EmailVerify.h"
//------------   end core     ------------//


// client info yaml file path
#ifdef WIN32
std::string yamlPath = "../../conf/clientInfo.yaml";
#else
std::string yamlPath = "../conf/clientInfo.yaml";
#endif

std::string CurSSID;
std::string CurSSname;

QWidget * curWindow = nullptr;

int MainLogic::startMainLogic() {
    g_pClientRequestHandler;
    eApp->init();

    // enable mica style
    eApp->setIsEnableMica(true);

    QString execPath = QApplication::applicationDirPath();

    QString BKStrategyPluginName;
#ifdef QT_DEBUG
    BKStrategyPluginName = "/EffectComponentPlugind.dll";
#else
    BKStrategyPluginName = "/EffectComponentPlugin.dll";
#endif
    // init Background strategy plugin
    if(g_pPluginManager->loadPlugin(BK_PLUGIN_NAME,execPath + BKStrategyPluginName)){
        // copy bk source from landpage rc to bin
        QDir tmpDir;
        if(!tmpDir.exists(execPath + "/assets")){
            tmpDir.mkpath(execPath + "/assets");
            //            g_pLandPage->saveQrcToFile(":/land-page/rc-page/video/bk_src.mp4",
            //                                       execPath + "/assets/bk_src.mp4");
        }

        // init effect bk source
        dynamic_cast<CVProVideoStrategy*>(g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME))->initialize(
                execPath + "/assets/bk_src.mp4"
        );
        g_pLandPage->show();
        curWindow = g_pLandPage;

        connect(g_pClientRequestHandler,&ClientRequestHandler::sigConnServerFailed,this,[=]() {
            ElaMessageBar::error(ElaMessageBarType::Top,"错误","无法连接到远程服务器!",6000, curWindow);
        });

        // TODO : link to core code
        // trigger verify account request
        connect(g_pLandPage,&LandPage::sigSignInRequest,this,[=](const QString& SSID,const QString& password){
            // TODO: ignored request handler
            g_pLandPage->close();
            g_pLandPage->destroyLandPage();

            // go to arch page
            g_pArchPage->show();
            curWindow = g_pArchPage;
        });
        connect(g_pLandPage, &LandPage::sigCurrentWidChanged,this,[=](QWidget* wid){curWindow = wid;});

        // verify email
        connect(g_pSignUpPage,&SignUpPage::sigEmailCodeRequest,this,[=](const QString & emailAddr) {
            g_pEmailVerify->sendEmailVerifyCode(emailAddr.toStdString(),GetCurTime::getTimeObj()->getCurTime());
        });
        connect(g_pClientRequestHandler,&ClientRequestHandler::sigEmailCodeResponse,this,[=](const std::string & dto) {
            g_pEmailVerify->parseEmailVerifyCode(dto);
        });

        // sign up
        connect(g_pSignUpPage,&SignUpPage::sigSignUpRequest,this,[=](const SignUpDataStruct & data) {

        });
        connect(g_pClientRequestHandler, &ClientRequestHandler::sigEnrollAccountResponse, this, [=](const std::string &dto) {

        });
    }
    return QApplication::exec();
}
