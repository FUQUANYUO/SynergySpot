#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "MysqlConn.h"


// 适用于 stringstream 中的 << 操作符
#define SQL_STR(__VAR__) \
    "'" << __VAR__ << "'"


/* eg :
 * A{c=1,d=2,e=3};
 * sqlStream << "UPDATE `table_name` SET ";
 * string c = A.c;
 * IF_MEMBER_EXIST(sqlStream,c,string)
 * sqlStream << "WHERE id='" << A.id <<"'";
 * */
#define IF_MEMBER_EXIST(__SQL__,__VAR__,__TYPE__)                   \
    if(__TYPE__ == "s"){                                            \
        if(!__VAR__.empty()) {                                      \
            __SQL__<< #__VAR__ << "='" << __VAR__ << "',";          \
        }else{                                                      \
            __SQL__<< #__VAR__ << "=" << #__VAR__ << ",";           \
        }                                                           \
    }else if(__TYPE__ == "i"){                                      \
        if(__VAR__ != "0"){                                         \
            __SQL__<< #__VAR__ << "=" << __VAR__ << ",";            \
        }else{                                                      \
            __SQL__<< #__VAR__ << "=" << #__VAR__ << ",";           \
        }                                                           \
    }else if(__TYPE__ == "se"){                                     \
        if(!__VAR__.empty()) {                                      \
            __SQL__<< #__VAR__ << "='" << __VAR__ << "' ";          \
        }else{                                                      \
            __SQL__<< #__VAR__ << "=" << #__VAR__ << " ";           \
        }                                                           \
    }else if(__TYPE__ == "ie"){                                     \
        if(__VAR__ != "0"){                                         \
            __SQL__<< #__VAR__ << "=" << __VAR__ << " ";            \
        }else{                                                      \
            __SQL__<< #__VAR__ << "=" << #__VAR__ << " ";           \
        }                                                           \
    }

class ConnectionPool
{
public:
    static ConnectionPool* getConnectPool();
    ConnectionPool(const ConnectionPool& obj) = delete;
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;
    std::shared_ptr<MysqlConn> getConnection();
private:
    ConnectionPool();
    bool parseYamlFile();
    void produceConnection();
    void recycleConnection();
    void addConnection();

    std::string m_ip;             // 数据库服务器ip地址
    std::string m_user;           // 数据库服务器用户名
    std::string m_dbName;         // 数据库服务器的数据库名
    std::string m_passwd;         // 数据库服务器密码
    unsigned short m_port;   // 数据库服务器绑定的端口
    int m_minSize;           // 连接池维护的最小连接数
    int m_maxSize;           // 连接池维护的最大连接数
    int m_maxIdleTime;       // 连接池中连接的最大空闲时长
    int m_timeout;           // 连接池获取连接的超时时长

    std::atomic_int m_connection_cnt;   // 现在拥有的连接数

    std::queue<MysqlConn*> m_connectionQ;
    std::mutex m_mutexQ;
    std::condition_variable m_cond;
};

