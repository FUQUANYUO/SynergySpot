//
// Created by FUQAQ on 2025/1/12.
//

#include "RealtimeCommHandler.h"
#include "yaml-cpp/yaml.h"

#include "help.h"

#include <QTimer>

namespace RealtimeComm {
    RealtimeCommHandler::RealtimeCommHandler(QObject* parent)
        : QObject(parent)
        , _isCallActive(false)
        , _isHeartbeatActive(false)
        , _coolDownTime(30 * 1000)
        , _heartbeatTimer(new QTimer(this))
    {
        // initWebRTC();
        setupGRPCChannel();

        // 设置心跳定时器
        connect(_heartbeatTimer, &QTimer::timeout, this, &RealtimeCommHandler::sltCheckHeartbeat);

        _heartbeatTimer->setInterval(_coolDownTime);
        _heartbeatTimer->start();
        _isHeartbeatActive = true;
    }

    RealtimeCommHandler::~RealtimeCommHandler() {
        // if (_isCallActive) {
        //     endVideoCall();
        // }
    }

    int RealtimeCommHandler::startGrpcService() {
        return QCoreApplication::exec();
    }

    void RealtimeCommHandler::setupGRPCChannel() {
        std::string yamlPath = g_pCommonData->getYamlPath();
        YAML::Node conf = YAML::LoadFile(yamlPath);
        if(!conf.IsNull()){
            std::string ip = conf["realtime-info"]["grpcIP"].as<std::string>();
            std::string port = conf["realtime-info"]["grpcPort"].as<std::string>();
            _maxRetryCount = conf["realtime-info"]["maxRetryCount"].as<int>();
            _channel = grpc::CreateChannel(ip + ":" + port, grpc::InsecureChannelCredentials());
            _stub = MediaService::NewStub(_channel);
        }
        else{
            LOG_INFO("set up grpc channel failed! conf is null, please check path! ")
        }
    }

    // bool RealtimeCommHandler::initVideoCall(const QString& remoteId) {
    //     if (_isCallActive) {
    //         qWarning() << "Call already in progress";
    //         return false;
    //     }
    //
    //     _currentRemoteId = remoteId;
    //     startChildProcess();
    //
    //     // 创建新的WebRTC连接
    //     _webrtc->createPeerConnection();
    //     _webrtc->setRemoteDescription(remoteId);
    //
    //     _isCallActive = true;
    //     emit signalCallStateChanged(1); // 1 = 通话开始
    //     return true;
    // }

    // void RealtimeCommHandler::endVideoCall() {
    //     if (!_isCallActive) return;
    //
    //     _webrtc->closePeerConnection();
    //     if (_mediaProcess) {
    //         _mediaProcess->terminate();
    //         _mediaProcess->waitForFinished();
    //     }
    //
    //     _isCallActive = false;
    //     _currentRemoteId.clear();
    //     emit signalCallStateChanged(0); // 0 = 通话结束
    // }

    void RealtimeCommHandler::sltCheckHeartbeat() {
        // 发送心跳包到服务器
        if (_isHeartbeatActive) {
            HeartbeatRequest request;
            request.set_client_id(g_pCommonData->getCurUserInfo().CurSSID);
            request.set_timestamp(GetCurTime::getTimeObj()->getCurTimeStamp());

            HeartbeatResponse response;
            grpc::ClientContext context;

            // 调用 gRPC 服务
            grpc::Status status = _stub->HeartbeatHandler(&context, request, &response);

            // 处理响应
            if (status.ok()) {
                bool alive = response.alive();
                LOG_INFO("Heartbeat response: alive = " << alive)
            } else {
                static int count = 0;
                count++;
                LOG_INFO("RPC failed:" << status.error_message())
                if (count > _maxRetryCount) {
                    LOG_INFO("the progress over cause of try count has exceeded max retry count")
                    _isHeartbeatActive = false;
                    QCoreApplication::exit(1);
                }
            }
        }
    }

} // namespace RealtimeComm