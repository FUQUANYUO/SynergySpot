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
    LiteConn(const char * dbName);
    bool update(const std::string sql);
    char ** query(const std::string &sql,int &nrow,int &ncolumn);
    ~LiteConn();
private:
    sqlite3 * qdb;
    char ** res = nullptr;
};


#endif//SYNERGYSPOT_LITECONN_H
