//
// Created by FU-QAQ on 2024/2/28.
//

#ifndef SYNERGYSPOT_LITECONN_H
#define SYNERGYSPOT_LITECONN_H

#include <SQLite/sqlite3.h>
#include "help.h"

class LiteConn {
public:
    LiteConn() = default;
    LiteConn(const char * dbName, std::string accessKey = "");

    bool beginTransaction();
    bool commit();
    bool rollback();

    bool update(const std::string& sql, const std::vector<std::string>& params);
    std::vector<std::vector<std::string>> query(const std::string& sql, const std::vector<std::string>& params = {});
    ~LiteConn();
private:
    sqlite3 * qdb;
    char ** res = nullptr;
};


#endif//SYNERGYSPOT_LITECONN_H
