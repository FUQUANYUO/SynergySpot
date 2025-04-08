#include "RealtimeCommHandler.h"

#include "trtc-realtime-comm/video-audio-call-page/VideoAudioInvitePage.h"
#include "trtc-realtime-comm/video-audio-call-page/VideoAudioCallPage.h"

#include <ela-widget-tools/ElaApplication.h>

#include "help.h"

#include <QApplication>

int main(int argc, char *argv[]){
    QApplication a(argc,argv);
    eApp->init();
    // enable mica style
    eApp->setIsEnableMica(true);

    std::string logName = "SynergySpot_gRPC";
    SSLog::initLogFile(logName);
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Beginning ----------------------------")
    RealtimeCommHandler realtime;
    int res = realtime.startGrpcService();
    LOG_INFO("--------------------------- SynergySpot-GRPC-Client.exe Ending ----------------------------")
    return res;
}