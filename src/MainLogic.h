//
// Created by FU-QAQ on 2024/12/10.
//

#ifndef SYNERGYSPOT_MAINLOGIC_H
#define SYNERGYSPOT_MAINLOGIC_H

#include <QObject>
#include <QApplication>

class QProcess;
class QLocalServer;
class QLocalSocket;

class MainLogic : public QObject{
    Q_OBJECT
public:
    explicit MainLogic();
    ~MainLogic();

    int startMainLogic(QApplication * app);

    bool initUserDataDir();
private:
    QProcess * _pGRPCProcess   = nullptr;
    QWidget  * _curWindow      = nullptr;
    QLocalServer * _pIPCServer = nullptr;
    QLocalSocket * _pGRCSocket = nullptr;
    const QString processName = "SynergySpot-GRPC-Client.exe";
};


#endif//SYNERGYSPOT_MAINLOGIC_H
