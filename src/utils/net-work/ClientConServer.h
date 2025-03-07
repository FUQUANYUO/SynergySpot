//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_CLIENTCONSERVER_H
#define SYNERGYSPOT_CLIENTCONSERVER_H

#include <QTcpSocket>
#include <QString>
#include <QQueue>

class QTimer;

class ClientConServer : public QObject{
    Q_OBJECT
public:
    explicit ClientConServer();

    QTcpSocket* getClientSocket();

    void enqueueData(const QByteArray &data);

    void connToSer();

    bool isConnected();

    ~ClientConServer();

    inline QString getServerIP() const { return _host_ip + ":" + QString::number(_port); }

private slots:
    void flushQueue();

private:

    QString _host_ip;
    qintptr _port;
    QTcpSocket _qSocket;

    QTimer  *_timer;
    QQueue<QByteArray> _queue;
    qint64 _maxPendingBytes;
};


#endif//SYNERGYSPOT_CLIENTCONSERVER_H
