#include "Server.h"
#include "ThreadPool.h"
#include "yaml-cpp/yaml.h"

#include "../src/protofile/DTO.pb.h"
#include "../src/utils/help.h"

#include "do-business/DoLogin.h"
#include "do-business/DoForwardMsg.h"

using namespace std;

std::string yamlPath = "../../src/conf/serverInfo.yaml";

//待发送的消息表      key: 发送人账号    v:发送的聊天记录
std::unordered_map<std::string, chatLog> sentLog;
//在线人员表  key:账号  v:与这个账号通信的Socket指针
std::unordered_map<std::string, TcpSocket *> onlineList;


void *working(void *arg) {
    struct SockInfo *pinfo = static_cast<struct SockInfo *>(arg);
    // 连接建立成功, 打印客户端的IP和端口信息
    char ip[32];
    printf("客户端的IP: %s, 端口: %d\n",
           inet_ntop(AF_INET, &pinfo->addr.sin_addr.s_addr, ip, sizeof(ip)),
           ntohs(pinfo->addr.sin_port));

    string ssid = "";
    // 5. 通信
    while (1) {
        string dto = "";
        char business_type;
        int res = pinfo->tcp->recvMsg(dto, business_type);
        if (!dto.empty() && res != 0) {
            //后续用epoll处理接收和发送
            //如果非空，获取数据，判断业务类型，之后调用对应的业务函数,服务器存储qq和对应的tcp
            if (business_type == SSDTO::Business_Type::LOGIN) {
                DoLogin dl;
                bool isPass = dl.execVerifyLogin(dto,ssid);
                string resDto = dl.sendVerifyRes(isPass);
                pinfo->tcp->sendMsg(resDto,SSDTO::Business_Type::LOGIN);

                if(isPass){
                    // 加入在线列表
                    onlineList[ssid] = pinfo->tcp;
                    DoForwardMsg::execForwardByMap(ssid);
                }else{
                    break;
                }
            }
            else if(business_type == SSDTO::Business_Type::FOWARD_MSG){
                DoForwardMsg dfmsg;
                dfmsg.execForward(dto);
            }
            else {
                LOG("some error occur in parse business!")
            }
        }
        else {
            break;
        }
    }
    LOG("user go offline")
    onlineList.erase(ssid);

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
