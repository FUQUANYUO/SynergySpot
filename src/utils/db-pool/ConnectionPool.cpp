#include "ConnectionPool.h"
#include "yaml-cpp/yaml.h"

#include <thread>

ConnectionPool* ConnectionPool::getConnectPool()
{
    static ConnectionPool pool;
    return &pool;
}

bool ConnectionPool::parseYamlFile()
{
    YAML::Node conf = YAML::LoadFile("../../conf/clientInfo.yaml");
    if (!conf.IsNull())
    {
        m_ip = conf["mysql-info"]["ip"].as<std::string>();
        m_port = conf["mysql-info"]["port"].as<int>();
        m_user = conf["mysql-info"]["userName"].as<std::string>();
        m_passwd = conf["mysql-info"]["password"].as<std::string>();
        m_dbName = conf["mysql-info"]["dbName"].as<std::string>();
        m_minSize = conf["mysql-info"]["minSize"].as<int>();
        m_maxSize = conf["mysql-info"]["maxSize"].as<int>();
        m_maxIdleTime = conf["mysql-info"]["maxIdleTime"].as<int>();
        m_timeout = conf["mysql-info"]["timeout"].as<int>();
        return true;
    }
    return false;
}

void ConnectionPool::produceConnection()
{
    while (true)
    {
        unique_lock<mutex> locker(m_mutexQ);
        while (!m_connectionQ.empty())
        {
            m_cond.wait(locker);
        }
        if(m_connection_cnt >= m_maxSize)
            continue;
        addConnection();
        m_cond.notify_all();
    }
}

void ConnectionPool::recycleConnection()
{
    while (true)
    {
        this_thread::sleep_for(chrono::milliseconds(m_maxIdleTime));
        lock_guard<mutex> locker(m_mutexQ);
        while (m_connection_cnt)
        {
            MysqlConn* conn = m_connectionQ.front();
            if (conn->getAliveTime() >= m_maxIdleTime)
            {
                m_connectionQ.pop();
                delete conn;
                --m_connection_cnt;
            }
            else
            {
                break;
            }
        }
    }
}

void ConnectionPool::addConnection()
{
    MysqlConn* conn = new MysqlConn;
    conn->connect(m_ip, m_user, m_passwd, m_dbName, m_port);
    conn->refreshAliveTime();
    m_connectionQ.push(conn);
    ++m_connection_cnt;
}

shared_ptr<MysqlConn> ConnectionPool::getConnection()
{
    unique_lock<mutex> locker(m_mutexQ);
    while (m_connectionQ.empty())
    {
        if (cv_status::timeout == m_cond.wait_for(locker, chrono::milliseconds(m_timeout)))
        {
            if (m_connectionQ.empty())
            {
                continue;
            }
        }
    }
    shared_ptr<MysqlConn> connptr(m_connectionQ.front(), [this](MysqlConn* conn) {
        lock_guard<mutex> locker(m_mutexQ);
        conn->refreshAliveTime();
        m_connectionQ.push(conn);
        });
    m_connectionQ.pop();
    m_cond.notify_all();
    return connptr;
}

ConnectionPool::~ConnectionPool()
{
    while (!m_connectionQ.empty())
    {
        MysqlConn* conn = m_connectionQ.front();
        m_connectionQ.pop();
        delete conn;
    }
}

ConnectionPool::ConnectionPool()
{
    if (!parseYamlFile())
    {
        return;
    }

    for (int i = 0; i < m_minSize; ++i)
    {
        addConnection();
    }
    thread producer(&ConnectionPool::produceConnection, this);
    thread recycler(&ConnectionPool::recycleConnection, this);
    producer.detach();
    recycler.detach();
}
