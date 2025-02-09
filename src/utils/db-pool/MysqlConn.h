#pragma once
#include "mysql/mysql.h"
#include <chrono>
#include <iostream>
using namespace std::chrono;

#include "../help.h"

class MysqlConn
{
public:
    struct Param {
        enum Type { INT, DOUBLE, STRING, BLOB };
        Type type;
        union {
            long long int_val;
            double double_val;
        };
        std::string str_val;
    };

    // 初始化数据库连接
    MysqlConn();
    // 释放数据库连接
    ~MysqlConn();
    // 连接数据库
    bool connect(std::string ip, std::string user, std::string passwd, std::string dbName, unsigned short port = 3306);
    // 更新操作（支持参数化）
    bool update(const std::string& sql, const std::vector<Param>& params = {});
    // 查询操作（支持参数化）
    MYSQL_RES* query(const std::string& sql, const std::vector<Param>& params = {});
    // 事务操作
    bool transaction();
    // 提交事务
    bool commit();
    // 事务回滚 
    bool rollback();
    // 刷新起始的空闲时间点
    void refreshAliveTime();
    // 计算连接存活的总时长
    long long getAliveTime();
    // 返回最后一次插入的 ID
    uint64_t getLastInsertId() const;
private:
    bool _bind_params(MYSQL_STMT* stmt, const std::vector<Param>& params);
private:
    MYSQL* m_conn = nullptr;
    steady_clock::time_point m_alivetime;
};

