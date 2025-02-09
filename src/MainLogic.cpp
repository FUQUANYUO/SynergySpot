//
// Created by FU-QAQ on 2024/12/10.
//
#include "MainLogic.h"

#include "base/client-request-handler/ClientRequestHandler.h"
#include "common-data/CommonData.h"

#include "land-page/LandPage.h"
#include "land-page/sign-up-page/SignUpPage.h"
#include "land-page/recover-pw-page/RecoverPWPage.h"
#include "plugin-manager/StrategyManager.h"
#include "effect-component/cv-process-video-strategy/CVProVideoStrategy.h"
#include "arch-page/ArchPage.h"

#include "ela-widget-tools/ElaApplication.h"
#include "ela-widget-tools/ElaMessageBar.h"

//------------ begin protobuf ------------//
#include "DTO.pb.h"
//------------ end protobuf   ------------//

//------------   begin core   ------------//
#include "safety/email-check/EmailVerify.h"
#include "safety/encrypt-passwd/EncryptPasswd.h"

#include "yaml-cpp/yaml.h"
//------------   end core     ------------//

#include <QProcess>
#include <QTimer>
#include <contact-page/ContactPage.h>
#include <message-page/MessagePage.h>

MainLogic::MainLogic() {
    std::string yamlPath = g_pCommonData->getYamlPath();

    YAML::Node config = YAML::LoadFile(yamlPath);
    std::string logName = config["log"]["logName"].as<std::string>();

    // init log orient
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot.exe Beginning ----------------------------")
}

MainLogic::~MainLogic() {
    g_pCommonData->destroyInstance();
    g_pEmailVerify->destroyInstance();
    g_pPluginManager->destroyInstance();
    g_pClientRequestHandler->destroyInstance();
    g_pSignUpPage->destroyInstance();
    LOG_INFO("--------------------------- SynergySpot.exe Ending ----------------------------")
    if (_pGRPCProcess && _pGRPCProcess->state() == QProcess::Running) {
        _pGRPCProcess->terminate();
        if (!_pGRPCProcess->waitForFinished(5000)) {
            _pGRPCProcess->kill();
        }
        if (!_pGRPCProcess->waitForFinished(1000)) {
             LOG_ERROR("!!! 无法终止进程，进程仍在运行  !!!");
        }
    }
    delete _pGRPCProcess;
}

int MainLogic::startMainLogic(QApplication * app) {
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
        _curWindow = g_pLandPage;

        connect(g_pClientRequestHandler,&ClientRequestHandler::sigConnServerFailed,this,[=]() {
            ElaMessageBar::error(ElaMessageBarType::Top,"错误","无法连接到远程服务器!",6000, _curWindow);
        });
        connect(g_pClientRequestHandler,&ClientRequestHandler::sigStartGRPCService,this,[=]() {
            QString processName = "SynergySpot-GRPC-Client.exe";
            _pGRPCProcess = new QProcess(app);
            _pGRPCProcess->start(processName,{QString::fromStdString(g_pCommonData->getCurUserInfo().CurSSID),
                                                          QString::fromStdString(g_pCommonData->getCurUserInfo().CurSSname),
                                                          QString::fromStdString(g_pCommonData->getCurUserInfo().CurUserAvatarPath)});
            connect(_pGRPCProcess,&QProcess::finished,this,[=](int exitCode, QProcess::ExitStatus exitStatus) {
                if(exitStatus == QProcess::NormalExit) {
                    if (exitCode != 0) {
                        ElaMessageBar::error(ElaMessageBarType::Top,"错误","GRPC 服务端连接出现错误！!",12000, _curWindow);
                    }
                }
            });
        });

        // TODO : link to core code
        // trigger verify account request
        connect(g_pLandPage,&LandPage::sigSignInRequest,this,[=](const QString& SSID,const QString& password){
            // TODO: ignored request handler

            // TODO: test simulate back end data for contact plugin
            // init user data dir
            g_pCommonData->setCurUserInfo({"100001","小柴",":/message-page/rc-page/img/default-avatar-1.jpg"});
            g_pMessagePage->loadCacheMsg(g_pCommonData->getMessageContentData(99,1));
            g_pContactPage->loadCacheContact(g_pCommonData->getCurUserFriendship());

            // go to arch page
            g_pArchPage->show();
            _curWindow = g_pArchPage;
            QTimer::singleShot(0, this, [=]() {
                g_pLandPage->close();
                g_pLandPage->destroyInstance();
            });
        });
        connect(g_pLandPage, &LandPage::sigCurrentWidChanged,this,[=](QWidget* wid){_curWindow = wid;});

        // verify email
        connect(g_pSignUpPage,&SignUpPage::sigEmailCodeRequest,this,[=](const QString & emailAddr) {
            g_pEmailVerify->sendEmailVerifyCode(emailAddr.toStdString(),GetCurTime::getTimeObj()->getCurTime());
        });
        connect(g_pClientRequestHandler,&ClientRequestHandler::sigEmailCodeResponse,this,[=](const std::string & dto) {
            g_pEmailVerify->parseEmailVerifyCode(dto);
        });

        // sign up
        connect(g_pSignUpPage,&SignUpPage::sigSignUpRequest,this,[=](const SignUpDataStruct & data) {
            std::string pdSalt = EncryptPasswd::generatePasswdSalt();
            SSDTO::EnrollAccountDTO edto;
            edto.set_ssid("-1");
            edto.set_user_name(data.name);
            edto.set_password(EncryptPasswd::encrypt(data.password,pdSalt));
            edto.set_email(data.email);
            edto.set_password_salt(pdSalt);

            std::string resDTO;
            edto.SerializeToString(&resDTO);
            g_pClientRequestHandler->sigEnrollAccountRequest(resDTO);
        });
        connect(g_pClientRequestHandler, &ClientRequestHandler::sigEnrollAccountResponse, this, [=](const std::string &dto) {
            SSDTO::EnrollAccountDTO edto;
            edto.ParseFromString(dto);
            LOG(edto.ssid());
        });

        // contact trigger msg add tmp card info
        connect(g_pContactPage,&ContactPage::sigTriggerAddMsgCard,g_pMessagePage,&MessagePage::addMsgCard);
    }
    return QApplication::exec();
}
