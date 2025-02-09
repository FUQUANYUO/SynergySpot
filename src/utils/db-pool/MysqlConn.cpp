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
        // // 设置事务隔离级别为 REPEATABLE READ
        // if (mysql_query(m_conn, "SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ") != 0) {
        //     LOG("Failed to set transaction isolation level: " << mysql_error(m_conn));
        //     return false;
        // }
        return true;
    }
    LOG("DataBase Connection Failed");
    LOG(mysql_error(m_conn))
    return false;
}

bool MysqlConn::update(const std::string &sql, const std::vector<Param> &params) {
    MYSQL_STMT* stmt = mysql_stmt_init(m_conn);
    if (!stmt) {
        std::cerr << "Statement init failed: " << mysql_error(m_conn) << std::endl;
        return false;
    }

    // 准备 SQL 语句
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.size()) != 0) {
        std::cerr << "Prepare failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    // 绑定参数
    if (!_bind_params(stmt, params)) {
        mysql_stmt_close(stmt);
        return false;
    }

    // 执行语句
    if (mysql_stmt_execute(stmt) != 0) {
        std::cerr << "Execute failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return false;
    }

    // 清理资源
    mysql_stmt_close(stmt);
    return true;
}

MYSQL_RES * MysqlConn::query(const std::string &sql, const std::vector<Param> &params) {
    MYSQL_STMT* stmt = mysql_stmt_init(m_conn);
    if (!stmt) {
        std::cerr << "Statement init failed: " << mysql_error(m_conn) << std::endl;
        return nullptr;
    }

    // 准备 SQL 语句
    if (mysql_stmt_prepare(stmt, sql.c_str(), sql.size()) != 0) {
        std::cerr << "Prepare failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return nullptr;
    }

    // 绑定参数
    if (!_bind_params(stmt, params)) {
        mysql_stmt_close(stmt);
        return nullptr;
    }

    // 执行查询
    if (mysql_stmt_execute(stmt) != 0) {
        std::cerr << "Execute failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return nullptr;
    }

    // 将结果转换为 MYSQL_RES 格式（兼容原有接口）
    MYSQL_RES* result = mysql_stmt_result_metadata(stmt);
    if (!result) {
        std::cerr << "No result metadata: " << mysql_stmt_error(stmt) << std::endl;
        mysql_stmt_close(stmt);
        return nullptr;
    }

    // 存储结果集
    if (mysql_stmt_store_result(stmt) != 0) {
        std::cerr << "Store result failed: " << mysql_stmt_error(stmt) << std::endl;
        mysql_free_result(result);
        mysql_stmt_close(stmt);
        return nullptr;
    }

    // 返回结果集（注意：需要调用者释放资源）
    mysql_stmt_close(stmt);
    return result;
}

bool MysqlConn::_bind_params(MYSQL_STMT* stmt, const std::vector<Param>& params) {
    if (params.empty()) return true;

    // 创建参数绑定数组
    std::vector<MYSQL_BIND> binds(params.size());
    memset(binds.data(), 0, sizeof(MYSQL_BIND) * params.size());

    for (size_t i = 0; i < params.size(); ++i) {
        switch (params[i].type) {
            case Param::INT: {
                binds[i].buffer_type = MYSQL_TYPE_LONG;
                binds[i].buffer = const_cast<long long*>(&params[i].int_val);
                break;
            }
            case Param::DOUBLE: {
                binds[i].buffer_type = MYSQL_TYPE_DOUBLE;
                binds[i].buffer = const_cast<double*>(&params[i].double_val);
                break;
            }
            case Param::STRING:
            case Param::BLOB: {
                binds[i].buffer_type = (params[i].type == Param::STRING) ?
                    MYSQL_TYPE_STRING : MYSQL_TYPE_BLOB;
                binds[i].buffer = const_cast<char*>(params[i].str_val.c_str());
                binds[i].buffer_length = params[i].str_val.size();
                break;
            }
        }
    }

    if (mysql_stmt_bind_param(stmt, binds.data()) != 0) {
        std::cerr << "Bind params failed: " << mysql_stmt_error(stmt) << std::endl;
        return false;
    }
    return true;
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

