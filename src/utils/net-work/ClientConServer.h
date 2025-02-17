//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_CLIENTCONSERVER_H
#define SYNERGYSPOT_CLIENTCONSERVER_H

#include <QTcpSocket>
#include <QString>

class ClientConServer : public QObject{
    Q_OBJECT
public:
    explicit ClientConServer();
    QTcpSocket *getQSocket();
    void connToSer();
    ~ClientConServer();

    inline QString getServerIP() const { return _host_ip + ":" + QString::number(_port); }
private:
    QString _host_ip;
    qintptr _port;
    QTcpSocket _qSocket;
};


#endif//SYNERGYSPOT_CLIENTCONSERVER_H
