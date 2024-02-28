#include "Server.h"
#include "ThreadPool.h"
#include "yaml-cpp/yaml.h"

#include "../src/protofile/DTO.pb.h"
#include "../src/utils/help.h"

#include "do-business/DoLogin.h"

using namespace std;

std::string yamlPath = "../../src/conf/serverInfo.yaml";

void *working(void *arg) {
    struct SockInfo *pinfo = static_cast<struct SockInfo *>(arg);
    // 连接建立成功, 打印客户端的IP和端口信息
    char ip[32];
    printf("客户端的IP: %s, 端口: %d\n",
           inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    // 5. 通信
    while (1) {
        string dto;
        char business_type;
        pinfo->tcp->recvMsg(dto, business_type);
        if (!dto.empty()) {
            //后续用epoll处理接收和发送
            //如果非空，获取数据，判断业务类型，之后调用对应的业务函数,服务器存储qq和对应的tcp
            if (business_type == SSDTO::Business_Type::LOGIN) {
                DoLogin dl;
                string resDto = dl.sendVerifyRes(dl.execVerifyLogin(dto));
                LOG(resDto)
                pinfo->tcp->sendMsg(resDto);
            } else {
                LOG("some error occur in parse business!")
            }
        } else {
            break;
        }
    }
    delete pinfo->tcp;
    delete pinfo;
    return nullptr;
}

int main() {
    YAML::Node node = YAML::LoadFile(yamlPath);
    if (node.IsNull()) return -1;
    int listen_port = node["host-info"]["listen-port"].as<int>();
    // 1. 创建监听的套接字
    TcpServer s;
    // 2. 绑定本地的IP port并设置监听
    s.setListen(listen_port);
    // 3. 阻塞并等待客户端的连接

    chatLog r;
    string f = "123";
    list<string> list;
    list.push_back(f);
    r[f] = list;
    if (r.find(f) != r.end()) {
        std::cout << "Element inserted successfully." << std::endl;
    } else {
        std::cout << "Element insertion failed." << std::endl;
    }

    while (1) {
        SockInfo *info = new SockInfo;
        TcpSocket *tcp = s.acceptConn(&info->addr);
        if (tcp == nullptr) {
            cout << "重试...." << endl;
            continue;
        }
        // 创建子线程
        pthread_t tid;
        info->s = &s;
        info->tcp = tcp;

        pthread_create(&tid, NULL, working, info);
        pthread_detach(tid);
    }
    return 0;
}
