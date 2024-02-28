//
// Created by FU-QAQ on 2024/2/28.
//

#include "LiteConn.h"
LiteConn::LiteConn(const char *dbName) {
    int rc = sqlite3_open(dbName,&qdb);
    if(rc){
        LOG("can't open sqlite database : " << sqlite3_errmsg(qdb))
        sqlite3_close(qdb);
    }
}
bool LiteConn::update(const std::string sql) {
    char * errMsg = nullptr;
    int rc = sqlite3_exec(qdb,sql.c_str(), nullptr, nullptr,&errMsg);
    if(rc != SQLITE_OK){
        LOG("sqlite update error : " << errMsg)
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
char **LiteConn::query(const std::string &sql,int &nrow,int &ncolumn) {
    int rc = 0;
    char * errMsg = nullptr;
    rc = sqlite3_get_table(qdb,sql.c_str(),&res,&nrow,&ncolumn,&errMsg);
    if(rc != SQLITE_OK){
        LOG("sqlite query error : " << errMsg)
        return nullptr;
    }
    return res;
}
LiteConn::~LiteConn() {
    sqlite3_free_table(res);
    sqlite3_close(qdb);
}
