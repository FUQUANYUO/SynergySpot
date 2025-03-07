//
// Created by FU-QAQ on 2024/2/25.
//
#include "help.h"
#include "yaml-cpp/yaml.h"

#include "common-data/CommonData.h"

#include <QTimer>

#include "ClientConServer.h"

ClientConServer::ClientConServer() {
    YAML::Node conf = YAML::LoadFile(g_pCommonData->getYamlPath());
    if (!conf.IsNull()) {
        _host_ip = QString::fromStdString(conf["server-host-info"]["ip"].as<std::string>());
        _port = conf["server-host-info"]["port"].as<int>();
    } else {
        std::cout << "conf is null,please check path!" << std::endl;
    }

    _timer = new QTimer(this);

    // flush queue
    connect(_timer, &QTimer::timeout, this, &ClientConServer::flushQueue);

    _timer->start(100);

    _maxPendingBytes = 1024 * 10;

    _qSocket.setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 1024 * 64);
}
QTcpSocket *ClientConServer::getClientSocket() {
    return &_qSocket;
}

void ClientConServer::enqueueData(const QByteArray &data)
{
    _queue.enqueue(data);
    if (_queue.size() == 1) {
        flushQueue();
    }
}

void ClientConServer::connToSer() {
    _qSocket.connectToHost(_host_ip, _port);
}
bool ClientConServer::isConnected() {
    return _qSocket.state() == QAbstractSocket::ConnectedState;
}
ClientConServer::~ClientConServer() {
    _qSocket.deleteLater();
}
void ClientConServer::flushQueue(){
    if (!_queue.isEmpty() && _qSocket.state() == QAbstractSocket::ConnectedState) {
        // 检查socket待写数据是否在允许的阈值之内
        if (_qSocket.bytesToWrite() < _maxPendingBytes) {
            QByteArray data = _queue.dequeue();
            qint64 written = _qSocket.write(data);
            // 如果未写入完整，则将剩余部分重新放回队列头部
            if (written < data.size()) {
                QByteArray remaining = data.mid(written);
                _queue.prepend(remaining);
            }
        }
    }
}