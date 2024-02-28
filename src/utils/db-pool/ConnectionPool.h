#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "MysqlConn.h"
using namespace std;
class ConnectionPool
{
public:
    static ConnectionPool* getConnectPool();
    ConnectionPool(const ConnectionPool& obj) = delete;
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;
    shared_ptr<MysqlConn> getConnection();
    ~ConnectionPool();
private:
    ConnectionPool();
    bool parseYamlFile();
    void produceConnection();
    void recycleConnection();
    void addConnection();

    string m_ip;             // 数据库服务器ip地址
    string m_user;           // 数据库服务器用户名
    string m_dbName;         // 数据库服务器的数据库名
    string m_passwd;         // 数据库服务器密码
    unsigned short m_port;   // 数据库服务器绑定的端口
    int m_minSize;           // 连接池维护的最小连接数
    int m_maxSize;           // 连接池维护的最大连接数
    int m_maxIdleTime;       // 连接池中连接的最大空闲时长
    int m_timeout;           // 连接池获取连接的超时时长

    atomic<int> m_connection_cnt;   // 现在拥有的连接数

    queue<MysqlConn*> m_connectionQ;
    mutex m_mutexQ;
    condition_variable m_cond;
};

