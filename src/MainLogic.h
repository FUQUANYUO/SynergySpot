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

enum LoadPhase {
    INIT,
    BASE_DATA_LOADING,  // 基础数据加载阶段
    RESOURCE_LOADING,   // 资源加载阶段
    COMPLETE
};

class MainLogic : public QObject{
    Q_OBJECT
public:
    explicit MainLogic();
    ~MainLogic();

    int startMainLogic(QApplication * app);

    bool initUserDataDir();

    void processJsonFromGRPCClient(const QByteArray& packet);

    void checkAllDataLoaded();

    void baseDataHandler();
private:
    bool _enable = true;
    bool isInit  = true;
    int  _dataLoadCounter = 3;  // base info (1); new msg (2); contact(3);
    int  _pendingPic      = 0;
    LoadPhase  _loadPhase = INIT;

    QProcess * _pGRPCProcess         = nullptr;
    QTimer   * _toWaitRemoteMsgTimer = nullptr;
    QWidget  * _curWindow            = nullptr;
    QLocalServer * _pIPCServer       = nullptr;
    QLocalSocket * _pGRCSocket       = nullptr;
    const QString processName        = "SynergySpot-GRPC-Client.exe";
};


#endif//SYNERGYSPOT_MAINLOGIC_H
