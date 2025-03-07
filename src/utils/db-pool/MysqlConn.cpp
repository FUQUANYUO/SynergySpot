#include "MysqlConn.h"
#include <cstring>
#include <regex>

using namespace std;

MysqlConn::MysqlConn()
{
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn, "utf8mb4");
}

MysqlConn::~MysqlConn()
{
    if (m_conn != nullptr)
    {
        mysql_close(m_conn);
    }
}

bool MysqlConn::connect(
    std::string ip,
    std::string user,
    std::string passwd,
    std::string dbName,
    unsigned short port
) {
    // 如果已有连接，先关闭旧连接
    if (m_conn != nullptr) {
        mysql_close(m_conn);
        m_conn = nullptr;
    }

    // 初始化MySQL连接对象
    m_conn = mysql_init(nullptr);
    if (!m_conn) {
        std::cerr << "mysql_init() failed" << std::endl;
        return false;
    }

    // 设置连接字符集为utf8mb4（支持4字节字符）
    if (mysql_options(m_conn, MYSQL_SET_CHARSET_NAME, "utf8mb4") != 0) {
        std::cerr << "Set charset failed: " << mysql_error(m_conn) << std::endl;
        mysql_close(m_conn);
        m_conn = nullptr;
        return false;
    }

    // 设置连接超时选项（5秒）
    unsigned int connect_timeout = 5;
    mysql_options(m_conn, MYSQL_OPT_CONNECT_TIMEOUT, &connect_timeout);

    // 执行实际连接
    MYSQL* ret = mysql_real_connect(
        m_conn,          // MySQL对象
        ip.c_str(),      // 主机地址
        user.c_str(),    // 用户名
        passwd.c_str(),  // 密码
        dbName.c_str(),  // 数据库名
        port,            // 端口号
        nullptr,         // Unix socket（默认）
        0                // 客户端标志（默认）
    );

    // 检查连接结果
    if (ret == nullptr) {
        std::cerr << "Connection failed: " << mysql_error(m_conn) << std::endl;
        mysql_close(m_conn);
        m_conn = nullptr;
        return false;
    }

    // 验证数据库是否选择成功（当dbName非空时）
    if (!dbName.empty()) {
        if (mysql_select_db(m_conn, dbName.c_str())) {
            std::cerr << "Select database failed: " << mysql_error(m_conn) << std::endl;
            mysql_close(m_conn);
            m_conn = nullptr;
            return false;
        }
    }

    // 标记连接存活时间
    refreshAliveTime();
    return true;
}

bool MysqlConn::detectedSQLInjection(const std::vector<Param>& params)
{
    std::regex sql_injection_regex(
            R"((\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|EXEC|ALTER|CREATE|TRUNCATE)\b))",
            std::regex_constants::icase
        );

    for (const auto& param : params) {
        switch (param.type) {
            case Param::STRING: {
                // 检查字符串参数中的危险模式
                if (std::regex_search(param.str_val, sql_injection_regex)) {
                    LOG("Potential SQL injection detected: " << param.str_val);
                    return true;
                }
                break;
            }
            case Param::INT:
            case Param::DOUBLE:
                break;
        }
    }
    return false;
}

std::string MysqlConn::bindParams(const std::string& sql, const std::vector<Param>& params)
{
    if (detectedSQLInjection(params)) {
        LOG_ERROR("SQL injection detected, query or update aborted.");
        std::string str;
        for (int i = 0;i < params.size();i++) {
            str += params[i].str_val;
            if (i != params.size() - 1) {
                str += ",";
            }
        }
        LOG_INFO(str)
        return "-1";
    }
    std::string replaced_sql;
    size_t param_idx = 0;
    size_t last_pos = 0;

    for (size_t pos = 0; (pos = sql.find('?', last_pos)) != std::string::npos;) {
        replaced_sql += sql.substr(last_pos, pos - last_pos);
        if (param_idx >= params.size()) break;

        const auto& param = params[param_idx];
        std::string value;

        switch (param.type) {
            case Param::INT:
                value = std::to_string(param.int_val);
            break;
            case Param::DOUBLE: {
                std::ostringstream oss;
                oss << param.double_val;
                value = oss.str();
                break;
            }
            case Param::STRING: {
                char* escaped = new char[2 * param.str_val.length() + 1];
                mysql_real_escape_string(m_conn, escaped, param.str_val.c_str(), param.str_val.length());
                value = "'";
                value += escaped;
                value += "'";
                delete[] escaped;
                break;
            }
        }

        replaced_sql += value;
        last_pos = pos + 1;
        param_idx++;
    }

    replaced_sql += sql.substr(last_pos);
    return replaced_sql;
}



bool MysqlConn::update(const std::string& sql, const std::vector<Param>& params) {
    std::string replaceStr = bindParams(sql, params);
    if(replaceStr == "-1")
        return false;

    if (mysql_real_query(m_conn, replaceStr.c_str(), replaceStr.size()) != 0)
    {
        LOG_ERROR("UPDATE FAILED: " + sql + "," +  mysql_error(m_conn))
        return false;
    }
    return true;
}


MYSQL_RES* MysqlConn::query(const std::string &sql, const std::vector<Param>& params) {
    std::string replaceStr = bindParams(sql, params);
    if(replaceStr == "-1")
        return nullptr;

    if (mysql_real_query(m_conn, replaceStr.c_str(), replaceStr.size()) != 0)
    {
        LOG_ERROR("QUERY FAILED: " + sql + "," + mysql_error(m_conn))
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

uint64_t MysqlConn::getLastInsertId() const {
    if (!m_conn) {
        throw std::runtime_error("MySQL connection is not initialized.");
    }
    return mysql_insert_id(m_conn);
}

