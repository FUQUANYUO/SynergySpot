#include <nlohmann/json.hpp>
#include <thread>
#include <csignal>
#include <sys/wait.h>

#include "GeneralServer.h"
#include "thread-pool/ThreadPool.h"
#include "yaml-cpp/yaml.h"
#include "do-business/ProcessBusiness.h"

using namespace std;

std::string yamlPath = "../../../src/conf/serverInfo.yaml";

// 待发送的消息表      key: 发送人账号    v:发送的聊天记录
std::unordered_map<std::string, chatLog> sentLog;
// 在线人员表         key:账号          v:与这个账号通信的Socket指针
std::unordered_map<std::string, SockInfo *> onlineList;
// 文件描述符 : TcpSocket
std::unordered_map<int, std::shared_ptr<SockInfo>> fd_sockets;
// 离线业务处理       key: 发送人账号    v:业务包 & 业务类型
std::unordered_map<std::string,std::deque<std::pair<std::string,SSDTO::BusinessType>>> businessSent;
// 构造json工具map   使用在并发环境下
std::unordered_map<std::string,nlohmann::json> jmp;


std::mutex onlineListMutex;
std::mutex fdSocketsMutex;

void working(void *arg){
    string ssid;
    auto readArgs = reinterpret_cast<ReadArgs*>(arg);
    int &fd = readArgs->fd;
    EpollEngine &en = *readArgs->en;

    std::shared_ptr<SockInfo> info;
    {
        std::lock_guard<std::mutex> lock(fdSocketsMutex);
        auto it = fd_sockets.find(fd);
        if (it == fd_sockets.end()) {
            en.deleteEvent(fd);
            return;
        }
        info = it->second;
    }

    // 非阻塞读取数据到缓冲区
    char buffer[4096];
    ssize_t res = info->tcp->recvPartial(buffer, sizeof(buffer));
    if (res <= 0) {
        if (res < 0 && (errno != EAGAIN && errno != EWOULDBLOCK)) {
            std::lock_guard<std::mutex> lock1(onlineListMutex);
            std::lock_guard<std::mutex> lock2(fdSocketsMutex);
            if (!info->ssid.empty()) onlineList.erase(info->ssid);
            fd_sockets.erase(fd);
            en.deleteEvent(fd);
        }
        return;
    }

    ProcessBusiness _pb(fd,&en);

    // 追加数据到缓冲区
    info->recvBuffer.insert(info->recvBuffer.end(), buffer, buffer + res);

    // 解析完整请求
    std::string dto;
    int businessType;
    while (_pb.parseCompleteRequest(info->recvBuffer, dto, businessType)) {
        _pb.processBusiness(dto,businessType,info);
    }

    // 登录
    en.modifyEvent(fd, EPOLLIN | EPOLLET);
}

// 创建监听的套接字
TcpServer s;
ThreadPool * pool = nullptr;

// 信号处理函数和全局变量
volatile sig_atomic_t stop = 0;
void handleSignal(int sig) {
    stop = 1;
}

int main() {
    // 注册信号
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    // 启动子进程
    pid_t grpcPid = fork();
    if (grpcPid == 0) { // 子进程
        execl("./SynergySpot-GRPC-Server", "SynergySpot-GRPC-Server", nullptr);
        LOG_ERROR("execl failed");
        exit(EXIT_FAILURE);
    } else if (grpcPid < 0) { // fork失败
        LOG_ERROR("fork failed");
        return -1;
    }

    YAML::Node node = YAML::LoadFile(yamlPath);
    if (node.IsNull()) return -1;
    int listenPort = node["host-info"]["listenPort"].as<int>();
    int poolMin = node["thread-pool"]["minSize"].as<int>();
    int poolMax = node["thread-pool"]["maxSize"].as<int>();

    SSLog::initLogFile("SynergySpot-Server");

    // 创建线程池
    ThreadPool Pool(poolMin, poolMax);
    pool = &Pool;
    // 绑定本地的IP port并设置监听
    s.setListen(listenPort);
    EpollEngine en;
    // 添加服务端监听事件
    en.addEvent(s.getListenFD(),EPOLLIN);

    while (!stop) {
        // 等待服务端监听响应
        int numsOfReady = en.waitForEvents();
        if (numsOfReady == -1) {
            LOG("errno value: " << errno)
            // 处理错误情况，可能需要退出循环或重启epoll
        }
        else {
            en.handleEvents(numsOfReady);
        }
    }

    // 终止子进程
    kill(grpcPid, SIGTERM);
    waitpid(grpcPid, nullptr, 0);
    return 0;
}
