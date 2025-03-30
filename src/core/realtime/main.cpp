#include "RealtimeCommHandler.h"

#include "help.h"
#include "testWindow.h"
#include <string>

int main(int argc, char *argv[]){
    // QCoreApplication a(argc,argv);
    // std::string logName = "SynergySpot_gRPC";
    // SSLog::initLogFile(logName);
    // LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Beginning ----------------------------")
    // RealtimeCommHandler realtime;
    // int res = realtime.startGrpcService();
    // LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Ending ----------------------------")
    // return res;

    QApplication a(argc, argv);
    std::string logName = "SynergySpot_gRPC";
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Beginning ----------------------------")
    VideoWindow w;
    w.show();
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Ending ----------------------------")
    return a.exec();
}