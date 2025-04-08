//
// Created by FU-QAQ on 2024/12/10.
//
#include "MainLogic.h"

#include "base/client-request-handler/ClientRequestHandler.h"
#include "common-data/CommonData.h"

#include "contact-page/ContactPage.h"
#include "message-page/MessagePage.h"
#include "user-page/UserPage.h"
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

#include <QJsonDocument>
#include <QLocalServer>
#include <QLocalSocket>
#include <QProcess>
#include <QTimer>


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

int MainLogic::startMainLogic(QApplication *app) {
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
    if (!g_pPluginManager->loadPlugin(BK_PLUGIN_NAME, execPath + BKStrategyPluginName)) {
        LOG_ERROR(BK_PLUGIN_NAME << "dont find!!!")
        return -1;
    }
    // copy bk source from landpage rc to bin
    QDir tmpDir;
    if (!tmpDir.exists(execPath + "/assets")) {
        tmpDir.mkpath(execPath + "/assets");
        //            g_pLandPage->saveQrcToFile(":/land-page/rc-page/video/bk_src.mp4",
        //                                       execPath + "/assets/bk_src.mp4");
    }

    // init effect bk source
    dynamic_cast<CVProVideoStrategy *>(g_pPluginManager->getCurrentPlugin(BK_PLUGIN_NAME))->initialize(execPath + "/assets/bk_src.mp4");

    g_pLandPage->show();
    _curWindow = g_pLandPage;

    connect(g_pClientRequestHandler, &ClientRequestHandler::sigConnServerFailed, this, [=]() {
        ElaMessageBar::error(ElaMessageBarType::Top, "错误", "无法连接到远程服务器!", 600000, _curWindow);
        _enable = false;
        g_pLandPage->sltHideLoading();
    });
    connect(g_pClientRequestHandler, &ClientRequestHandler::sigStartGRPCService, this, [=]() {
        _pGRPCProcess = new QProcess(app);
        _pIPCServer = new QLocalServer(this);

        connect(_pGRPCProcess, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitStatus == QProcess::NormalExit) {
                if (exitCode != 0) {
                    ElaMessageBar::error(ElaMessageBarType::Top, "错误", "GRPC服务端连接出现错误，\n子进程连接已关闭！!", 60000, _curWindow);
                    g_pLandPage->sltHideLoading();
                }
            }
        });
        connect(_pIPCServer, &QLocalServer::newConnection, _pIPCServer, [&]() {
            _pGRCSocket = _pIPCServer->nextPendingConnection();
            if (!_pGRCSocket) {
                LOG_ERROR("Failed to get client socket.")
                return;
            }

            connect(_pGRCSocket, &QLocalSocket::readyRead, _pGRCSocket, [=]() {
                QByteArray response = _pGRCSocket->readAll();
                QJsonDocument doc = QJsonDocument::fromJson(response);
                if (!doc.isNull()) {
                    QJsonObject resp = doc.object();
                    if (resp["status"] == "success") {
                        FileStorageDTO responseFileDto;
                        responseFileDto.fileId = resp["file-id"].toString();
                        responseFileDto.uploaderSsid = resp["uploader-ssid"].toString();
                        responseFileDto.fileName = resp["file-name"].toString();
                        responseFileDto.fileSize = resp["file-size"].toInt();
                        responseFileDto.fileType = resp["file-type"].toString();
                        responseFileDto.storagePath = resp["local-path"].toString();
                        responseFileDto.uploadTime = resp["upload-time"].toInt();

                        // avatar
                        if (resp["business-type"].toString() == "avatar") {
                            UserBaseInfoDTO curInfo;
                            curInfo.ssid = responseFileDto.uploaderSsid;
                            curInfo.avatarPath = resp["local-path"].toString();
                            g_pCommonData->updateUserInfoBySSID(curInfo);
                            curInfo = g_pCommonData->getCurUserInfo();
                            if (curInfo.ssid == responseFileDto.uploaderSsid) {
                                curInfo.avatarPath = resp["local-path"].toString();
                                g_pCommonData->setCurUserInfo(curInfo);
                            }
                            emit g_pCommonData->sigUpdateAvatarData();

                            _dataLoadCounter--;
                            checkAllDataLoaded();
                        }
                        // msg pic
                        else if (resp["business-type"].toString() == "msg_pic") {
                            // if (resp["type"] == "file-upload") {
                            //
                            // }
                            if (resp["type"] == "file-download") {
                                emit g_pCommonData->sigUpdateMsgView();

                                _dataLoadCounter--;
                                checkAllDataLoaded();
                            }
                        }

                        // storage in local file info
                        if (g_pCommonData->getFileInfoById(resp["file-id"].toString()).fileId.isEmpty()) {
                            g_pCommonData->setFileInfo(responseFileDto);

                            _dataLoadCounter--;
                            checkAllDataLoaded();
                        }
                    }
                    else if (resp["status"] == "error" && resp["message"].toString().contains("grpc")) {
                        LOG(resp["message"].toString().toStdString())
                        ElaMessageBar::error(ElaMessageBarType::Top, "错误", "GRPC 服务端连接出现错误！!", 3000, _curWindow);
                    }
                    else if (resp["status"] == "error" && resp["type"].toString().contains("video-call")) {
                        int errorCode = resp["error-code"].toString().toInt();
                        if (errorCode == -1) {
                            ElaMessageBar::error(ElaMessageBarType::Top, "错误", "请关闭当前通话的窗口！!", 3000, _curWindow);
                        }else if (errorCode == -2) {
                            ElaMessageBar::warning(ElaMessageBarType::Top, "警告", "其他用户请求与您通话，但您还未关闭遗留通话！", 3000, _curWindow);
                        }
                    }
                    else {
                        LOG("file failed : " << resp["message"].toString().toStdString())
                        _dataLoadCounter--;
                        checkAllDataLoaded();
                    }
                }
            });
        });

        // start ipc
        if (!_pIPCServer->listen("SynergySpotIPC")) {
            LOG_ERROR("IPC start failed!")
        }

        // start grpc sub process
        _pGRPCProcess->start(processName);
    });

    // trigger verify account request
    connect(g_pLandPage, &LandPage::sigSignInRequest, this, [=](const QString &SSID, const QString &password) {
        do {
            if (!_enable) {
                LOG_ERROR("sign in request reject")
                ElaMessageBar::error(ElaMessageBarType::TopLeft, "错误", "无法连接到服务器!", 600000, _curWindow);
                break;
            }
            SSDTO::LoginCheckDTO ldto;
            ldto.set_ssid(SSID.toStdString());
            ldto.set_password(password.toStdString());
            ldto.set_is_pass(false);
            std::string resDTO;
            ldto.SerializeToString(&resDTO);
            emit g_pClientRequestHandler->sigVerifyAccountRequest(resDTO);

            // wait login result
            g_pLandPage->sltShowLoading();
        } while (false);
    });
    connect(g_pLandPage, &LandPage::sigCurrentWidChanged, this, [=](QWidget *wid) { _curWindow = wid; });
    connect(g_pClientRequestHandler, &ClientRequestHandler::sigLoginResult, this, [=](const std::string &dto) {
        g_pLandPage->isFreezeSignInBtn(false);

        SSDTO::LoginCheckDTO ldto;
        ldto.ParseFromString(dto);
        // login success
        if (ldto.is_pass()) {
            _dataLoadCounter = 0;

            // set user info in grpc sub process
            {
                QJsonObject userInfo;
                userInfo["command"] = "set";
                userInfo["ssid"] = QString::fromStdString(ldto.ssid());

                _pGRCSocket->write(QJsonDocument(userInfo).toJson());
                _pGRCSocket->flush();
            }

            // init user data dir
            g_pCommonData->setCurUserInfo({QString::fromStdString(ldto.ssid())});
            auto *pConnection = new QMetaObject::Connection;
            *pConnection = connect(g_pClientRequestHandler, &ClientRequestHandler::sigQueryUserBaseInfoResponse,
                this, [=](const std::string &dto) {
                    isInit = false;

                    // disconnect from event listen
                    QObject::disconnect(*pConnection);
                    delete pConnection;

                    // update user info
                    SSDTO::UserBaseInfoDTO udto;
                    udto.ParseFromString(dto);

                    UserBaseInfoDTO baseInfoDto{
                            QString::fromStdString(udto.ssid()),
                            QString::fromStdString(udto.ssname()),
                            "",
                            QString::fromStdString(udto.sex()),
                            QString::fromStdString(udto.personal_sign()),
                            udto.birthday(),
                            udto.thumb_up_count(),
                            static_cast<uint8_t>(udto.region()),
                            udto.create_time(),
                    };
                    FileStorageDTO res = g_pCommonData->getFileInfoById(QString::fromStdString(udto.avatar_file_id()));
                    if (res.fileId == "-1" || res.fileId.isEmpty()) {
                        // send query avatar by grpc
                        QJsonObject resp;
                        resp["command"] = "download";
                        resp["file-id"] = QString::fromStdString(udto.avatar_file_id());
                        resp["uploader-ssid"] = QString::fromStdString(udto.ssid());
                        resp["storage-path"] = QString::fromStdString(udto.avatar_remote_path());
                        resp["local-path"] = QString::fromStdString(g_pCommonData->getDataPath(avatar) + "/" + udto.avatar_file_id() + g_pCommonData->getImageEx());
                        resp["business-type"] = "avatar";
                        _pGRCSocket->write(QJsonDocument(resp).toJson());
                        _pGRCSocket->flush();
                        _dataLoadCounter++;
                    } else {
                        baseInfoDto.avatarPath = res.storagePath;
                    }

                    // refresh local cache
                    g_pCommonData->setCurUserInfo(baseInfoDto);
                    auto curUserInfo = g_pCommonData->getUserInfoBySSID(QString::fromStdString(ldto.ssid()));
                    if (curUserInfo.ssid == "-1" || curUserInfo.ssid.isEmpty()) {
                        g_pCommonData->addUserInfoByServer(baseInfoDto);
                    } else {
                        g_pCommonData->updateUserInfoBySSID(baseInfoDto);
                    }

                    // store user private data
                    g_pCommonData->setLoginRecord({QString::fromStdString(ldto.ssid()),
                                                  QString::fromStdString(ldto.password()),
                                                  QDateTime::currentMSecsSinceEpoch(),
#ifdef Q_OS_WIN
                                                  "WINDOWS"
#else
    "LINUX"
#endif
                    });

                    g_pArchPage->hide();

                    // refresh data
                    emit g_pCommonData->sigUpdateAvatarData();

                    _dataLoadCounter--;
                    checkAllDataLoaded();
               });
            connect(g_pClientRequestHandler, &ClientRequestHandler::sigQueryNewMessageResponse, this, [=](const std::string& dto) {
                SSDTO::GetUserMessageDTO gudto;
                gudto.ParseFromString(dto);

                QList<MessageContentDTO> newsMsg;
                for (const auto &msg: gudto.msg()) {
                    MessageContentDTO tmpDto;
                    tmpDto.senderSSID = QString::fromStdString(msg.sender_ssid());
                    tmpDto.contentType = static_cast<ContentType>(msg.content_type());
                    tmpDto.content = QString::fromStdString(msg.content());
                    tmpDto.recipient = {
                            qint32(msg.recipient().recipient_type()),
                            QString::fromStdString(msg.recipient().recipient_ssid()),
                            msg.recipient().read_status()};
                    tmpDto.createTime = msg.create_time();

                    for (const auto &fid: msg.file_id()) {
                        tmpDto.fileId.append(QString::fromStdString(fid));
                    }

                    QString curSSID = QString::fromStdString(ldto.ssid());
                    newsMsg.append(tmpDto);

                    SSDTO::UserBaseInfoDTO udto;
                    if (tmpDto.senderSSID != curSSID &&
                        g_pCommonData->getUserInfoBySSID(tmpDto.senderSSID).ssid.isEmpty())
                    {
                        udto.set_ssid(tmpDto.senderSSID.toStdString());
                        std::string resDto;
                        udto.SerializeToString(&resDto);
                        emit g_pClientRequestHandler->sigQueryUserBaseInfoRequest(resDto);
                        _dataLoadCounter++;
                    }
                    if (tmpDto.recipient.recipientSSID != curSSID &&
                        g_pCommonData->getUserInfoBySSID(tmpDto.recipient.recipientSSID).ssid.isEmpty())
                    {
                        udto.set_ssid(tmpDto.recipient.recipientSSID.toStdString());
                        std::string resDto;
                        udto.SerializeToString(&resDto);
                        emit g_pClientRequestHandler->sigQueryUserBaseInfoRequest(resDto);
                        _dataLoadCounter++;
                    }
                }
                if (!newsMsg.empty())
                    g_pCommonData->setMessageContentData(newsMsg, true);

                _dataLoadCounter--;
                checkAllDataLoaded();
            });

            connect(g_pClientRequestHandler, &ClientRequestHandler::sigContactListResponse, this, [=](const std::string dto) {
                SSDTO::GetAllUserFriendship gfdto;
                gfdto.ParseFromString(dto);

                QList<FriendshipDTO> contacts;
                QList<UserBaseInfoDTO> friendBaseInfos;
                for (const auto &info: gfdto.friends_base_info()) {
                    UserBaseInfoDTO baseInfoDto{
                            QString::fromStdString(info.ssid()),
                            QString::fromStdString(info.ssname()),
                            "",
                            QString::fromStdString(info.sex()),
                            QString::fromStdString(info.personal_sign()),
                            info.birthday(),
                            info.thumb_up_count(),
                            static_cast<uint8_t>(info.region()),
                            info.create_time(),
                    };

                    FileStorageDTO res = g_pCommonData->getFileInfoById(QString::fromStdString(info.avatar_file_id()));
                    if (res.fileId == "-1" || res.fileId.isEmpty()) {
                        // send query avatar by grpc
                        emit g_pCommonData->sigGetAvatarFileFromRemote(
                            QString::fromStdString(info.avatar_file_id()),
                            QString::fromStdString(info.ssid()),
                            QString::fromStdString(info.avatar_remote_path())
                        );
                        _dataLoadCounter++;
                    } else {
                        baseInfoDto.avatarPath = res.storagePath;
                    }
                    if (g_pCommonData->getUserInfoBySSID(baseInfoDto.ssid).ssid.isEmpty())
                        g_pCommonData->addUserInfoByServer(baseInfoDto);
                }
                for (const auto &info: gfdto.friendship_info()) {
                    contacts.append({QString::fromStdString(info.ssid()),
                                     QString::fromStdString(info.grouping()),
                                     QString::fromStdString(info.friend_ssid()),
                                     qint32(info.ship_status()),
                                     qint32(info.friend_type()),
                                     info.create_time()});
                }

                if (!contacts.empty())
                    g_pCommonData->setFriendshipData(contacts);

                _dataLoadCounter--;
                checkAllDataLoaded();
            });

            // get user base info
            {
                SSDTO::UserBaseInfoDTO udto;
                udto.set_ssid(ldto.ssid());
                std::string resDto;
                udto.SerializeToString(&resDto);
                emit g_pClientRequestHandler->sigQueryUserBaseInfoRequest(resDto);
            }

            // get new msg from server
            {
                SSDTO::GetUserMessageDTO gudto;
                gudto.set_ssid(ldto.ssid());
                gudto.set_last_time(std::to_string(g_pCommonData->getLastMessageTime()));
                gudto.set_page_size(99);
                gudto.set_page_num(1);
                std::string resUdto;
                gudto.SerializeToString(&resUdto);
                emit g_pClientRequestHandler->sigQueryNewMessageRequest(resUdto);
            }

            // get user contact
            {
                SSDTO::GetAllUserFriendship gfdto;
                gfdto.set_ssid(ldto.ssid());
                std::string resgfDto;
                gfdto.SerializeToString(&resgfDto);
                emit g_pClientRequestHandler->sigContactListRequest(resgfDto);
            }
        }
        // login failed
        else {
            g_pLandPage->clearPasswordInput();
            ElaMessageBar::error(ElaMessageBarType::Top, "错误", "账号或密码错误!", 6000, g_pLandPage);
            // close loading
            g_pLandPage->sltHideLoading();
        }
    });
    connect(g_pClientRequestHandler, &ClientRequestHandler::sigQueryUserBaseInfoResponse, this, [=](const std::string &dto) {
        if (isInit)// init get user base info dont trigger this func
            return;
        SSDTO::UserBaseInfoDTO udto;
        udto.ParseFromString(dto);
        if (g_pCommonData->getUserInfoBySSID(QString::fromStdString(udto.ssid())).ssid.isEmpty()) {
            g_pCommonData->addUserInfoByServer({
                    QString::fromStdString(udto.ssid()),
                    QString::fromStdString(udto.ssname()),
                    "",
                    QString::fromStdString(udto.sex()),
                    QString::fromStdString(udto.personal_sign()),
                    udto.birthday(),
                    udto.thumb_up_count(),
                    static_cast<uint8_t>(udto.region()),
                    udto.create_time(),
            });
        }
    });

    // verify email
    connect(g_pSignUpPage, &SignUpPage::sigEmailCodeRequest, this, [=](const QString &emailAddr) {
        g_pEmailVerify->sendEmailVerifyCode(emailAddr.toStdString(), GetCurTime::getTimeObj()->getCurTime());
    });
    connect(g_pClientRequestHandler, &ClientRequestHandler::sigEmailCodeResponse, this, [=](const std::string &dto) {
        g_pSignUpPage->sigEmailCodeResponse(QString::fromStdString(g_pEmailVerify->parseEmailVerifyCode(dto)));
    });

    // sign up
    connect(g_pSignUpPage, &SignUpPage::sigSignUpRequest, this, [=](const SignUpDataStruct &data) {
        do {
            if (!_enable) {
                LOG_ERROR("sign up page reject")
                ElaMessageBar::error(ElaMessageBarType::TopLeft, "错误", "无法连接到服务器，请检查网络!", 600000, _curWindow);
                break;
            }
            std::string pdSalt = EncryptPasswd::generatePasswdSalt();
            SSDTO::EnrollAccountDTO edto;
            edto.set_ssid("-1");
            edto.set_user_name(data.name);
            edto.set_password(EncryptPasswd::encrypt(data.password, pdSalt));
            edto.set_email(data.email);
            edto.set_password_salt(pdSalt);

            std::string resDTO;
            edto.SerializeToString(&resDTO);
            g_pClientRequestHandler->sigEnrollAccountRequest(resDTO);
        } while (false);
    });
    connect(g_pClientRequestHandler, &ClientRequestHandler::sigEnrollAccountResponse, this, [=](const std::string &dto) {
        SSDTO::EnrollAccountDTO edto;
        edto.ParseFromString(dto);
        LOG(edto.ssid());
    });

    // contact trigger msg add tmp card info
    connect(g_pContactPage, &ContactPage::sigTriggerAddMsgCard, g_pMessagePage, &MessagePage::addMsgCard);

    // cur user info changed
    connect(g_pUserPage(Myself, {}, {}), &UserPage::sigUserAvatarChanged, this, [=](const QString &localPath) {
        QJsonObject cmd;
        cmd["command"] = "upload";
        cmd["local-path"] = localPath;
        cmd["business-type"] = "avatar";
        cmd["uploader-ssid"] = g_pCommonData->getCurUserInfo().ssid;
        if (_pGRCSocket != nullptr) {
            _pGRCSocket->write(QJsonDocument(cmd).toJson());
            _pGRCSocket->flush();
            _dataLoadCounter++;
        }
    });

    // forward msg
    connect(g_pClientRequestHandler,&ClientRequestHandler::sigForwardMessageResponse,this,[=](const std::string &dto) {
        _dataLoadCounter--;
        checkAllDataLoaded();

        SSDTO::MessageContentDTO mdto;
        mdto.ParseFromString(dto);

        MessageContentDTO lmdto;
        lmdto.content = QString::fromStdString(mdto.content());
        lmdto.recipient = {
            qint32(mdto.recipient().recipient_type()),
            QString::fromStdString(mdto.recipient().recipient_ssid()),
            mdto.recipient().read_status()
        };
        lmdto.contentType = static_cast<ContentType>(mdto.content_type());
        lmdto.createTime = mdto.create_time();

        for (const auto& it : mdto.file_id()) {
            lmdto.fileId.append(QString::fromStdString(it));
        }

        lmdto.senderSSID = QString::fromStdString(mdto.sender_ssid());

        g_pCommonData->setMessageContentData({lmdto},true);
        g_pMessagePage->loadCacheMsg({lmdto});
    });

    // upload to server for msg dto
    connect(g_pCommonData, &CommonData::sigSyncMsgContentDTO, this, [=](const QList<MessageContentDTO> &dto) {
        _dataLoadCounter++;
        // send tmp pic to server
        for (const auto &msg: dto) {
            SSDTO::MessageContentDTO mdto;
            for (const auto &file: msg.fileId) {// one msg contains some pic file
                QJsonObject cmd;
                cmd["command"] = "upload";
                cmd["local-path"] = QString::fromStdString(g_pCommonData->getDataPath(msgPic)) + "/" + file +
                                   QString::fromStdString(g_pCommonData->getImageEx());
                cmd["business-type"] = "msg_pic";
                if (_pGRCSocket != nullptr) {
                    _pGRCSocket->write(QJsonDocument(cmd).toJson());
                    _pGRCSocket->flush();
                }
                mdto.add_file_id(file.toStdString());
            }
            mdto.set_sender_ssid(msg.senderSSID.toStdString());
            mdto.set_content_type(static_cast<int>(msg.contentType));
            mdto.set_content(msg.content.toStdString());
            mdto.mutable_recipient()->set_recipient_type(static_cast<int>(msg.recipient.recipientType));
            mdto.mutable_recipient()->set_recipient_ssid(msg.recipient.recipientSSID.toStdString());
            mdto.mutable_recipient()->set_read_status(msg.recipient.readStatus);

            std::string resDto;
            mdto.SerializeToString(&resDto);
            emit g_pClientRequestHandler->sigForwardMessageRequest(resDto);
        }
    });
    // download msg pic from remote
    connect(g_pCommonData, &CommonData::sigSyncMsgPicFromRemote, this, [=](const QList<QString> &files) {
        auto *pConnection = new QMetaObject::Connection;
        *pConnection = connect(g_pClientRequestHandler, &ClientRequestHandler::sigQueryMsgPicInfoResponse,
           this, [=](const std::string &dto) {
               // disconnect from event listen
               QObject::disconnect(*pConnection);
               delete pConnection;

               SSDTO::GetMessagePicInfoDTO gmpdto;
               gmpdto.ParseFromString(dto);

               for (const auto &fileInfo: gmpdto.pic_name_to_path()) {
                   QJsonObject cmd;
                   cmd["command"] = "download";
                   cmd["business-type"] = "msg_pic";
                   cmd["local-path"] = QString::fromStdString(g_pCommonData->getDataPath(msgPic) + "/" +
                                                             fileInfo.first + g_pCommonData->getImageEx());
                   cmd["file-id"] = QString::fromStdString(fileInfo.first);
                   cmd["storage-path"] = QString::fromStdString(fileInfo.second);

                   if (_pGRCSocket != nullptr) {
                       _pGRCSocket->write(QJsonDocument(cmd).toJson());
                       _pGRCSocket->flush();
                       _dataLoadCounter++;
                   }
               }
               _dataLoadCounter --;
               checkAllDataLoaded();
           });

        // query file info
        {
            SSDTO::GetMessagePicInfoDTO gmpdto;
            gmpdto.set_ssid(g_pCommonData->getCurUserInfo().ssid.toStdString());
            auto _map = gmpdto.mutable_pic_name_to_path();
            for (const auto &file: files) {
                (*_map)[file.toStdString()] = "";
            }

            std::string resDto;
            gmpdto.SerializeToString(&resDto);
            emit g_pClientRequestHandler->sigQueryMsgPicInfoRequest(resDto);
            _dataLoadCounter++;
        }
    });

    // all data load from server database or local database
    connect(g_pCommonData, &CommonData::sigAllDataLoadFinished, this, [=]() {
        // close loading
        g_pLandPage->sltHideLoading();

        g_pMessagePage->loadCacheMsg(g_pCommonData->getMessageContentData(99, 1));
        g_pContactPage->loadCacheContact(g_pCommonData->getCurUserFriendship());

        // goto arch page
        g_pArchPage->show();
        _curWindow = g_pArchPage;

        g_pLandPage->close();
        g_pLandPage->destroyInstance();

        _dataLoadCounter = INT_MAX;
    });

    // fuzzy search
    connect(g_pCommonData,&CommonData::sigFuzzySearchRequest,this,[=](const QString& content,bool isGroup) {
        SSDTO::FuzzySearchDTO fuzzyDto;
        fuzzyDto.set_is_group(isGroup);

        bool isSSID;
        content.toInt(&isSSID);
        if (isSSID) {
            fuzzyDto.set_ssid(content.toStdString());
        }else {
            fuzzyDto.set_name(content.toStdString());
        }
        std::string resDto;
        fuzzyDto.SerializeToString(&resDto);

        g_pClientRequestHandler->sigFuzzySearchRequest(resDto);
    });

    connect(g_pClientRequestHandler,&ClientRequestHandler::sigFuzzySearchResponse,[=](const std::string& dto) {
        int waitCount = 0;
        SSDTO::FuzzySearchDTO fuzzyDto;
        fuzzyDto.ParseFromString(dto);
        bool isGroup = fuzzyDto.is_group();
        if (!isGroup) {
            QList<UserBaseInfoDTO> resList;
            for (const auto& info : fuzzyDto.user_infos()) {
                UserBaseInfoDTO baseInfoDto{
                    QString::fromStdString(info.ssid()),
                    QString::fromStdString(info.ssname()),
                    "",
                    QString::fromStdString(info.sex()),
                    QString::fromStdString(info.personal_sign()),
                    info.birthday(),
                    info.thumb_up_count(),
                    static_cast<uint8_t>(info.region()),
                    info.create_time(),
                };

                FileStorageDTO res = g_pCommonData->getFileInfoById(QString::fromStdString(info.avatar_file_id()));
                if (res.fileId == "-1" || res.fileId.isEmpty()) {
                    // send query avatar by grpc
                    emit g_pCommonData->sigGetAvatarFileFromRemote(
                        QString::fromStdString(info.avatar_file_id()),
                        QString::fromStdString(info.ssid()),
                        QString::fromStdString(info.avatar_remote_path())
                    );
                    waitCount++;
                } else {
                    baseInfoDto.avatarPath = res.storagePath;
                }
                if (g_pCommonData->getUserInfoBySSID(baseInfoDto.ssid).ssid.isEmpty())
                    g_pCommonData->addUserInfoByServer(baseInfoDto);
                resList.append(baseInfoDto);
            }
            emit g_pCommonData->sigFuzzySearchFriendResponse(resList,waitCount);
        }
        else {
            QList<GroupBaseInfoDTO> resList;
            for (const auto& info : fuzzyDto.group_infos()) {
                GroupBaseInfoDTO baseInfoDto{
                    QString::fromStdString(info.ssid_group()),
                    QString::fromStdString(info.name()),
                    "",
                    QString::fromStdString(info.create_ssid()),
                    QString::fromStdString(info.profile()),
                    info.create_time(),
                };

                // FileStorageDTO res = g_pCommonData->getFileInfoById(QString::fromStdString(info.avatar_file_id()));
                // if (res.fileId == "-1" || res.fileId.isEmpty()) {
                //     // send query avatar by grpc
                //     emit g_pCommonData->sigGetAvatarFileFromRemote(
                //         QString::fromStdString(info.avatar_file_id()),
                //         QString::fromStdString(info.ssid()),
                //         QString::fromStdString(info.avatar_remote_path())
                //     );
                //     waitCount++;
                // } else {
                //     baseInfoDto.avatarPath = res.storagePath;
                // }
                if (g_pCommonData->getGroupInfoDataBySSID(baseInfoDto.ssidGroup).ssidGroup.isEmpty())
                    g_pCommonData->setGroupInfoData({baseInfoDto});
                resList.append(baseInfoDto);
            }
            emit g_pCommonData->sigFuzzySearchGroupResponse(resList,waitCount);
        }
    });

    // add friend request
    connect(g_pCommonData,&CommonData::sigAddFriendOrGroup,this,[=](const QString& ssid, bool isGroup) {
        SSDTO::MakeFriendDTO mdto;
        mdto.set_sender(g_pCommonData->getCurUserInfo().ssid.toStdString());
        mdto.set_recipient(ssid.toStdString());
        mdto.set_accept(false);
        mdto.set_isgroup(isGroup);

        std::string resDto;
        mdto.SerializeToString(&resDto);

        emit g_pClientRequestHandler->sigAddFriendRequest(resDto);
        if (isGroup)
            emit g_pContactPage->sigAddJoinGroupRecord(g_pCommonData->getGroupInfoDataBySSID(ssid),NoticeStatus::Waiting);
        else
            emit g_pContactPage->sigAddMakeFriendRecord(g_pCommonData->getUserInfoBySSID(ssid),NoticeStatus::Waiting);
    });

    // other user accept be friend response / request of be friend from other user
    connect(g_pClientRequestHandler,&ClientRequestHandler::sigFriendRequestResponse,this,[=](const std::string& dto) {
        SSDTO::MakeFriendDTO mdto;
        mdto.ParseFromString(dto);
        std::string curSSID = g_pCommonData->getCurUserInfo().ssid.toStdString();

        UserBaseInfoDTO userInfoRes;
        GroupBaseInfoDTO groupInfoRes;

        if (mdto.sender() != curSSID) { // 添加请求信息
            // TODO: 先缓存用户信息
            if (mdto.isgroup()) {
                groupInfoRes = g_pCommonData->getGroupInfoDataBySSID(QString::fromStdString(mdto.sender()));
                if (groupInfoRes.ssidGroup.isEmpty() || groupInfoRes.ssidGroup == "-1") {
                    groupInfoRes.ssidGroup = QString::fromStdString(mdto.sender());
                    groupInfoRes.avatarPath = "-1";
                }
                emit g_pContactPage->sigAddJoinGroupRecord(
                    groupInfoRes,NoticeStatus::Request);
            }else {
                userInfoRes = g_pCommonData->getUserInfoBySSID(QString::fromStdString(mdto.sender()));
                if (userInfoRes.ssid.isEmpty() || userInfoRes.ssid == "-1") {
                    userInfoRes.ssid = QString::fromStdString(mdto.sender());
                    userInfoRes.avatarPath = "-1";
                }
                emit g_pContactPage->sigAddMakeFriendRecord(
                    userInfoRes,NoticeStatus::Request);
            }
        }
        else { // 更新 Waiting 状态
            if (mdto.isgroup()) {
                groupInfoRes = g_pCommonData->getGroupInfoDataBySSID(QString::fromStdString(mdto.recipient()));
                if (groupInfoRes.ssidGroup.isEmpty() || groupInfoRes.ssidGroup == "-1") {
                    groupInfoRes.ssidGroup = QString::fromStdString(mdto.recipient());
                    groupInfoRes.avatarPath = "-1";
                }
                if (mdto.accept())
                    emit g_pContactPage->sigAddJoinGroupRecord(
                        groupInfoRes,NoticeStatus::Accepted);
                else
                    emit g_pContactPage->sigAddJoinGroupRecord(
                        groupInfoRes,NoticeStatus::Rejected);
            }
            else {
                userInfoRes = g_pCommonData->getUserInfoBySSID(QString::fromStdString(mdto.recipient()));
                if (userInfoRes.ssid.isEmpty() || userInfoRes.ssid == "-1") {
                    userInfoRes.ssid = QString::fromStdString(mdto.recipient());
                    userInfoRes.avatarPath = "-1";
                }
                if (mdto.accept())
                    emit g_pContactPage->sigAddMakeFriendRecord(
                        userInfoRes,NoticeStatus::Accepted);
                else
                    emit g_pContactPage->sigAddMakeFriendRecord(
                        userInfoRes,NoticeStatus::Rejected);
            }
        }
    });

    connect(g_pCommonData,&CommonData::sigReplyFriendOrGroup,this,
        [=](const QString& ssid,bool isAccept, bool isGroup)
    {
        SSDTO::MakeFriendDTO mdto;
        mdto.set_accept(isAccept);
        mdto.set_isgroup(isGroup);
        mdto.set_sender(ssid);
        mdto.set_recipient(g_pCommonData->getCurUserInfo().ssid.toStdString());

        std::string resDto;
        mdto.SerializeToString(&resDto);
        emit g_pClientRequestHandler->sigAddFriendRequest(resDto);
    });

    // get avatar file
    connect(g_pCommonData,&CommonData::sigGetAvatarFileFromRemote,this,[=](
            const QString& fileID,
            const QString& ssid,
            const QString& remotePath)
    {
        QJsonObject resp;
        resp["command"] = "download";
        resp["file-id"] = fileID;
        resp["uploader-ssid"] = ssid;
        resp["storage-path"] = remotePath;
        resp["local-path"] = QString::fromStdString(g_pCommonData->getDataPath(avatar)) + "/" + fileID + QString::fromStdString(g_pCommonData->getImageEx());
        resp["business-type"] = "avatar";
        _pGRCSocket->write(QJsonDocument(resp).toJson());
        _pGRCSocket->flush();
    });

    // video call request
    connect(g_pCommonData,&CommonData::sigCallVideoToOtherUser,this,[=](const QString& remoteSSID) {
        SSDTO::VideoCallDTO vcallDto;
        vcallDto.set_sender_ssid(g_pCommonData->getCurUserInfo().ssid);
        vcallDto.set_target_ssid(remoteSSID.toStdString());
        vcallDto.set_user_sig("");

        std::string resDto;
        vcallDto.SerializeToString(&resDto);
        emit g_pClientRequestHandler->sigCallVideoRequest(resDto);
    });

    // video call response
    connect(g_pClientRequestHandler,&ClientRequestHandler::sigCallVideoResponse,this,[=](const std::string& dto) {
        SSDTO::VideoCallDTO vcallDto;
        vcallDto.ParseFromString(dto);

        auto curUserInfo = g_pCommonData->getCurUserInfo();

        QJsonObject resp;
        resp["command"]         = "video-call";
        // other user call video to you
        if (curUserInfo.ssid.toStdString() != vcallDto.sender_ssid()) {
            resp["target-ssid"]     = QString::fromStdString(vcallDto.sender_ssid());
            resp["is-other-invite"] = true;
            resp["user-sig"]        = QString::fromStdString(vcallDto.user_sig());
        }
        // user-sig come from server
        else {
            resp["target-ssid"]     = QString::fromStdString(vcallDto.target_ssid());
            resp["is-other-invite"] = false;
            resp["user-sig"]        = QString::fromStdString(vcallDto.user_sig());
        }
        _pGRCSocket->write(QJsonDocument(resp).toJson());
        _pGRCSocket->flush();
    });

    return QApplication::exec();
}

void MainLogic::checkAllDataLoaded() {
    if (_dataLoadCounter <= 0) {
        emit g_pCommonData->sigAllDataLoadFinished();
    }
}
