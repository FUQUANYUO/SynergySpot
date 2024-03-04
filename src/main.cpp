#include <QApplication>
#include <QDebug>
#include <iostream>
#include <qt_windows.h>

#include "page/land-page/LandPage.h"
#include "page/base-arch/arch-page/ArchPage.h"
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

#include "db-lite/LiteConn.h"

extern std::string CurSSID;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LandPage lp;
    lp.show();

    // 缓存数据测试
//    LiteConn lcon("test.db");

    // 创建表
//    string createFriendshipTable = R"(DROP TABLE IF EXISTS friendship;
//                                      CREATE TABLE friendship (
//                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
//                                            ssid TEXT NOT NULL,
//                                            friend_ssid TEXT NOT NULL,
//                                            ship_status INTEGER NOT NULL,
//                                            create_time DATETIME DEFAULT CURRENT_TIMESTAMP
//                                    );)";
//    string createGroupMemberTable = R"(DROP TABLE IF EXISTS group_member_info;
//                                       CREATE TABLE group_member_info (
//                                            id INTEGER PRIMARY KEY AUTOINCREMENT,
//                                            ssid_group TEXT NOT NULL,
//                                            ssid_member TEXT NOT NULL,
//                                            create_time DATETIME DEFAULT CURRENT_TIMESTAMP
//                                    );)";
//    string createUserMsgTable = R"(DROP TABLE IF EXISTS user_message;
//                                   CREATE TABLE user_message (
//                                        id INTEGER PRIMARY KEY AUTOINCREMENT,
//                                        friendship_id TEXT,
//                                        group_member_id TEXT,
//                                        content TEXT NOT NULL,
//                                        FOREIGN KEY (friendship_id) REFERENCES friendship(id),
//                                        FOREIGN KEY (group_member_id) REFERENCES group_member_info(id)
//                                );)";
//    if(lcon.update(createFriendshipTable)){
//        LOG("success create table")
//    }
//    if(lcon.update(createGroupMemberTable)){
//        LOG("success create table")
//    }
//    if(lcon.update(createUserMsgTable)){
//        LOG("success create table")
//    }

    // 插入数据
//    string insertSql = R"(INSERT INTO friendship(`ssid`,`friend_ssid`,`ship_status`) VALUES('1001','1002',0);)";
//    if(lcon.update(insertSql)){
//        LOG("success insert into val")
//    }
//    insertSql = R"(INSERT INTO group_member_info(`ssid_group`,`ssid_member`) VALUES('900','1001');)";
//    if(lcon.update(insertSql)){
//        LOG("success insert into val")
//    }
//    insertSql = R"(INSERT INTO user_message(`friendship_id`,`content`) VALUES(3,"this is test2");)";
//    if(lcon.update(insertSql)){
//        LOG("success insert into val")
//    }

//    CurSSID = "18745496";
//    ArchPage pg;
//    pg.show();
    return QApplication::exec();
}
