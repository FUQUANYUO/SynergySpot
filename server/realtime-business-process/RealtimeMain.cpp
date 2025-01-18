//
// Created by FUQAQ on 2025/1/14.
//

#include <csignal>

#include "RealtimeBusinessProcess.h"
#include "yaml-cpp/yaml.h"

#include <help.h>

std::string yamlPath = "../../../src/conf/serverInfo.yaml";

std::unique_ptr<RealtimeBusinessProcess> businessProcess;

void closeSignalHandler(int sig) {
    if (businessProcess.get() != nullptr) {
        businessProcess->Stop();
        LOG_INFO("Extern give signal to stopping RealtimeBusinessProcess...");
    }
}

int main(){
    YAML::Node config = YAML::LoadFile(yamlPath);
    std::string address = config["realtime-info"]["grpcIP"].as<std::string>() + ":" + config["realtime-info"]["grpcPort"].as<std::string>();

    SSLog::initLogFile("SynergySpot-GRPC-Server");

    businessProcess = std::make_unique<RealtimeBusinessProcess>(address);

    std::signal(SIGINT,  closeSignalHandler);
    std::signal(SIGTERM, closeSignalHandler);

    businessProcess->Start();

    businessProcess->Wait();

    return 0;
}