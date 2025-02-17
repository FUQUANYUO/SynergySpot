#include "RealtimeCommHandler.h"

#include <string>
#include "help.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc,argv);
    std::string logName = "SynergySpot_gRPC";
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Beginning ----------------------------")

    g_pCommonData->setCurUserInfo({argv[1], argv[2], argv[3]});

    RealtimeComm::RealtimeCommHandler realtime;
    int res = realtime.startGrpcService();
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Ending ----------------------------")
    return res;
}