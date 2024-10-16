//
// Created by FU-QAQ on 2024/2/25.
//
#include "help.h"
#include "yaml-cpp/yaml.h"

#include "ClientConServer.h"

ClientConServer::ClientConServer() {
    YAML::Node conf = YAML::LoadFile(yamlPath);
    if(!conf.IsNull()){
        _host_ip = QString::fromStdString(conf["server-host-info"]["ip"].as<std::string>());
        _port = conf["server-host-info"]["port"].as<int>();
    }
    else{
        std::cout << "conf is null,please check path!" << std::endl;
    }
}

QTcpSocket *ClientConServer::getQSocket() {
    return &_qSocket;
}
void ClientConServer::connToSer() {
    _qSocket.connectToHost(_host_ip,_port);
}
ClientConServer::~ClientConServer() {
    _qSocket.deleteLater();
}
