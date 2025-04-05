#include "RealtimeCommHandler.h"

#include "TRTCTestWindow.h"
#include <string>
#include "help.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc,argv);
    std::string logName = "SynergySpot_gRPC";
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Beginning ----------------------------")
    // RealtimeCommHandler realtime;
    // int res = realtime.startGrpcService();
    TRTCTestWindow testWindow;
    testWindow.show();
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Ending ----------------------------")
    return a.exec();
}