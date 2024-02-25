//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_CLIENTCONSERVER_H
#define SYNERGYSPOT_CLIENTCONSERVER_H

#include <QTcpSocket>
#include <QString>

class ClientConServer {
public:
    explicit ClientConServer();
    QTcpSocket *getQSocket();
    void ConnToSer();
    ~ClientConServer() = default;
private:
    QString _host_ip;
    qintptr _port;
    QTcpSocket _qSocket;
};


#endif//SYNERGYSPOT_CLIENTCONSERVER_H
