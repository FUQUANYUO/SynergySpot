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
#include <QLocalServer>
#include <QLocalSocket>
#include <QFileDialog>
#include <QJsonDocument>
#include <QLocalServer>
#include <QTimer>
#include <contact-page/ContactPage.h>
#include <message-page/MessagePage.h>
#include <user-page/UserPage.h>

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

        // get data from server and load cache
        g_pMessagePage->loadCacheMsg(g_pCommonData->getMessageContentData(99,1));
        g_pContactPage->loadCacheContact(g_pCommonData->getCurUserFriendship());

        g_pLandPage->show();
        _curWindow = g_pLandPage;

        connect(g_pClientRequestHandler,&ClientRequestHandler::sigConnServerFailed,this,[=]() {
            ElaMessageBar::error(ElaMessageBarType::Top,"错误","无法连接到远程服务器!",6000, _curWindow);
        });
        connect(g_pClientRequestHandler,&ClientRequestHandler::sigStartGRPCService,this,[=]() {
            _pGRPCProcess = new QProcess(app);
            _pIPCServer   = new QLocalServer(this);

            connect(_pGRPCProcess,&QProcess::finished,this,[=](int exitCode, QProcess::ExitStatus exitStatus) {
                if(exitStatus == QProcess::NormalExit) {
                    if (exitCode != 0) {
                        ElaMessageBar::error(ElaMessageBarType::Top,"错误","GRPC 服务端连接出现错误！!",12000, _curWindow);
                    }
                }
            });
            connect(_pIPCServer,&QLocalServer::newConnection,_pIPCServer,[&]() {
                _pGRCSocket = _pIPCServer->nextPendingConnection();
                if (!_pGRCSocket) {
                     LOG_ERROR("Failed to get client socket.")
                    return ;
                }

                connect(_pGRCSocket,&QLocalSocket::readyRead,_pGRCSocket,[=]() {
                    QByteArray response = _pGRCSocket->readAll();
                    QJsonDocument doc = QJsonDocument::fromJson(response);
                    if (!doc.isNull()) {
                        QJsonObject resp = doc.object();
                        if (resp["status"] == "success") {
                            if (resp["type"] == "file-upload" && resp.contains("file-id")) {
                                LOG("test file uuid : " << resp["file-id"].toString().toStdString())
                            }
                            if (resp["type"] == "file-download" && resp.contains("local-path")) {
                                LOG("test file at : " << resp["local-path"].toString().toStdString())
                            }
                        }else {
                            LOG("test file failed : " << resp["message"].toString().toStdString())
                        }
                    }
                });
            });

            // start ipc
            if (!_pIPCServer->listen("SynergySpotIPC")) {
                LOG_ERROR("IPC start failed!")
            }

            _pGRPCProcess->start(processName,{"100000","绅士柴",""});
        });

        // trigger verify account request
        connect(g_pLandPage,&LandPage::sigSignInRequest,this,[=](const QString& SSID,const QString& password){
            SSDTO::LoginCheckDTO ldto;
            ldto.set_ssid(SSID.toStdString());
            ldto.set_password(password.toStdString());
            ldto.set_is_pass(false);
            std::string resDTO;
            ldto.SerializeToString(&resDTO);
            g_pClientRequestHandler->sigVerifyAccountRequest(resDTO);
            if (SSID == "121212" || password == "1") {
                // init user data dir
                // UserBaseInfoDTO user = g_pCommonData->getUserInfoBySSID(SSID);
                // if (user.ssid == "-1") {
                //     // load animation
                //     emit g_pClientRequestHandler->sigQueryUserBaseInfoRequest(SSID.toStdString());
                // }
                // TODO: async from server data
                g_pCommonData->setCurUserInfo({"100000","绅士柴",""});

                // go to arch page
                g_pArchPage->show();
                _curWindow = g_pArchPage;

                QString filePath = QFileDialog::getOpenFileName(
                    nullptr,
                    "选择文件",
                    QDir::homePath(),
                    "所有文件 (*);;"
                );
                QJsonObject cmd;
                cmd["command"] = "upload";
                cmd["local-url"] = filePath;
                if (_pGRCSocket != nullptr) {
                    _pGRCSocket->write(QJsonDocument(cmd).toJson());
                    _pGRCSocket->flush();
                }

                QTimer::singleShot(0, this, [=]() {
                    g_pLandPage->close();
                    g_pLandPage->destroyInstance();
                });
            }
        });
        connect(g_pLandPage, &LandPage::sigCurrentWidChanged,this,[=](QWidget* wid){_curWindow = wid;});
        connect(g_pClientRequestHandler, &ClientRequestHandler::sigLoginFailed,this,[=]() {
            g_pLandPage->clearPasswordInput();
            ElaMessageBar::error(ElaMessageBarType::Top,"错误","账号或密码错误!",6000, g_pLandPage);
        });
        connect(g_pClientRequestHandler, &ClientRequestHandler::sigLoginSuccess,this,[=](const std::string& ssid) {
            // init user data dir
            UserBaseInfoDTO user = g_pCommonData->getUserInfoBySSID(QString::fromStdString(ssid));
            if (user.ssid == "-1") {
                // load animation
                emit g_pClientRequestHandler->sigQueryUserBaseInfoRequest(ssid);
            }
            g_pCommonData->setCurUserInfo(user);

            // go to arch page
            g_pArchPage->show();
            _curWindow = g_pArchPage;
            QTimer::singleShot(0, this, [=]() {
                g_pLandPage->close();
                g_pLandPage->destroyInstance();
            });
        });
        connect(g_pClientRequestHandler, &ClientRequestHandler::sigQueryUserBaseInfoResponse,this,[=](const std::string& dto) {
            // cancel load animation
            SSDTO::UserBaseInfoDTO udto;
            udto.ParseFromString(dto);
            g_pCommonData->addUserInfoByServer({
                QString::fromStdString(udto.ssid()),
                QString::fromStdString(udto.ssname()),
                QString::fromStdString(udto.avatar_path()),
                QString::fromStdString(udto.sex()),
                QString::fromStdString(udto.personal_sign()),
                QDateTime::fromMSecsSinceEpoch(udto.birthday()),
                udto.thumb_up_count(),
                static_cast<uint8_t>(udto.region()),
                QDateTime::fromMSecsSinceEpoch(udto.create_time()),
            });
        });

        // verify email
        connect(g_pSignUpPage,&SignUpPage::sigEmailCodeRequest,this,[=](const QString & emailAddr) {
            g_pEmailVerify->sendEmailVerifyCode(emailAddr.toStdString(),GetCurTime::getTimeObj()->getCurTime());
        });
        connect(g_pClientRequestHandler,&ClientRequestHandler::sigEmailCodeResponse,this,[=](const std::string & dto) {
            g_pSignUpPage->sigEmailCodeResponse(QString::fromStdString(g_pEmailVerify->parseEmailVerifyCode(dto)));
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
