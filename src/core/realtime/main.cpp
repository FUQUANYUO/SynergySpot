#include "RealtimeCommHandler.h"

#include <string>
#include "help.h"

std::string CurSSID;

#ifdef WIN32
std::string yamlPath = "../../conf/clientInfo.yaml";
#else
std::string yamlPath = "../conf/clientInfo.yaml";
#endif

int main(int argc, char *argv[]){
    QCoreApplication a(argc,argv);
    std::string logName = "SynergySpot_gRPC";
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot-GPRC-Client.exe Beginning ----------------------------")
    // CurSSID = argv[1];
    RealtimeComm::RealtimeCommHandler realtime;
    int res = realtime.startGrpcService();
    LOG_INFO("--------------------------- SynergySpot-GPRC-Client.exe Ending ----------------------------")
    return res;
}