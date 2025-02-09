//
// Created by FU-QAQ on 2024/2/28.
//

#include "LiteConn.h"

LiteConn::LiteConn(const char *dbName, std::string accessKey) {
    if (accessKey == "") {
        int rc = sqlite3_open(dbName,&qdb);
        if(rc){
            LOG("can't open sqlite database : " << sqlite3_errmsg(qdb))
            sqlite3_close(qdb);
        }
    }else {
        int rc = sqlite3_open(dbName,&qdb);
        if(rc) {
            LOG("can't open sqlite database : " << sqlite3_errmsg(qdb))
            sqlite3_close(qdb);
        }
        int res = sqlite3_key(qdb, accessKey.c_str(), strlen(accessKey.c_str()));
        if(res != SQLITE_OK) {
            LOG_ERROR("cant set key for sqlites3 : " << sqlite3_errmsg(qdb))
            sqlite3_close(qdb);
        }
    }
    sqlite3_exec(qdb, "PRAGMA foreign_keys = ON;", 0, 0, 0);
}

bool LiteConn::beginTransaction() {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(qdb, "BEGIN TRANSACTION", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Begin transaction failed: " << errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool LiteConn::commit() {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(qdb, "COMMIT", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Commit failed: " << errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool LiteConn::rollback() {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(qdb, "COMMIT", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Commit failed: " << errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool LiteConn::update(const std::string& sql, const std::vector<std::string>& params) {
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(qdb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Prepare failed: " << sqlite3_errmsg(qdb));
        return false;
    }

    // 绑定参数
    for (size_t i = 0; i < params.size(); i++) {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    // 执行 SQL
    bool result = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!result) {
        LOG_ERROR("Update failed: " << sqlite3_errmsg(qdb));
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<std::vector<std::string>> LiteConn::query(const std::string& sql, const std::vector<std::string>& params) {
    std::vector<std::vector<std::string>> result;
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(qdb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Prepare failed: " << sqlite3_errmsg(qdb));
        return result;
    }

    // 绑定参数
    for (size_t i = 0; i < params.size(); i++) {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    // 获取列数
    int ncolumn = sqlite3_column_count(stmt);

    // 遍历结果集
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < ncolumn; i++) {
            const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
            row.push_back(value ? value : "");
        }
        result.push_back(row);
    }
    sqlite3_finalize(stmt);
    return result;
}

LiteConn::~LiteConn() {
    sqlite3_free_table(res);
    sqlite3_close(qdb);
}
