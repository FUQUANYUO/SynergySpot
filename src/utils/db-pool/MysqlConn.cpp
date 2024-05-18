#include "MysqlConn.h"

using namespace std;

MysqlConn::MysqlConn()
{
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn, "utf8");
}

MysqlConn::~MysqlConn()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
}

bool MysqlConn::connect(string ip, string user, string passwd, string dbName, unsigned short port)
{
    MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
    if(ptr != nullptr){
        return true;
    }
    LOG("DataBase Connection Failed");
    LOG(mysql_error(m_conn))
    return false;
}

bool MysqlConn::update(const string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG("UPDATE FAILED: " + sql)
        LOG(mysql_error(m_conn))
        return false;
    }
    return true;
}

MYSQL_RES* MysqlConn::query(const string sql)
{
    if (mysql_query(m_conn, sql.c_str()))
    {
        LOG("QUERY FAILED: " + sql);
        LOG(mysql_error(m_conn))
        return nullptr;
    }
    return mysql_store_result(m_conn);
}

bool MysqlConn::transaction()
{
    return mysql_autocommit(m_conn, false);
}

bool MysqlConn::commit()
{
    return mysql_commit(m_conn);
}

bool MysqlConn::rollback()
{
    return mysql_rollback(m_conn);
}

void MysqlConn::refreshAliveTime()
{
    m_alivetime = steady_clock::now();
}

long long MysqlConn::getAliveTime()
{
    nanoseconds res = steady_clock::now() - m_alivetime;
    milliseconds millsec = duration_cast<milliseconds>(res);
    return millsec.count();
}
