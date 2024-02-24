#include <QApplication>
#include <QDebug>
#include <iostream>

#include "page/land-page/LandPage.h"
#include "db-pool/ConnectionPool.h"

#include "SQLite/sqlite3.h"

void GetQueryRes(MYSQL_RES * res,const vector<int> &index){
    MYSQL_ROW row;
    while((row = mysql_fetch_row(res))) {
        std::cout << "| ";
        for (auto it: index) {
            std::cout << row[it] << " | ";
        }
        std::cout << std::endl;
    }
    mysql_free_result(res);
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LandPage lp;
    lp.show();
// 数据库测试
//    ConnectionPool::getConnectPool()->getConnection()->update("insert into user_private_info(`ssid`,`status`,`password`) values('10000001',0,'Fu159753')");
//    GetQueryRes(ConnectionPool::getConnectPool()->getConnection()->query("SELECT `ssid`,`status`,`password` FROM user_private_info"),{0,1,2});
    return QApplication::exec();
}
