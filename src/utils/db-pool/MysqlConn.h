#pragma once
#include "mysql/mysql.h"
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

#include "../help.h"

class MysqlConn
{
public:
    // 初始化数据库连接
    MysqlConn();
    // 释放数据库连接
    ~MysqlConn();
    // 连接数据库
    bool connect(string ip, string user, string passwd, string dbName, unsigned short port = 3306);
    // 更新数据库: insert, update, delete
    bool update(const string sql);
    // 查询数据库
    MYSQL_RES * query(const string sql);
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
private:
    MYSQL* m_conn = nullptr;
    steady_clock::time_point m_alivetime;
};

