//#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QGuiApplication>

//#include "page/land-page/LandPage.h"
//#include "page/base-arch/arch-page/ArchPage.h"
//#include "db-pool/ConnectionPool.h"

//std::string yamlPath = "../../conf/clientInfo.yaml";

using namespace std;

//void GetQueryRes(MYSQL_RES * res,const vector<int> &index){
//    MYSQL_ROW row;
//    while((row = mysql_fetch_row(res))) {
//        std::cout << "| ";
//        for (auto it: index) {
//            std::cout << row[it] << " | ";
//        }
//        std::cout << std::endl;
//    }
//    mysql_free_result(res);
//}

int main(int argc, char *argv[]) {
//    QApplication a(argc, argv);
//    a.setStyleSheet(R"(
//        QMainWindow,QWidget {
//            background-color: white; /* 设置背景为白色 */
//            color: black; /* 设置文本颜色为黑色 */
//        }
//        QPushButton {
//            background-color: #e0e0e0; /* 设置按钮背景为浅灰色 */
//        }
//        QLineEdit, QTextEdit, QPlainTextEdit, QListView, QTreeView, QTableView {
//            background-color: white; /* 设置输入控件和视图背景为白色 */
//            color: black; /* 设置输入控件和视图文本为黑色 */
//            selection-background-color: #bbdefb; /* 设置选中项的背景颜色 */
//        }
//    )");
//    LandPage lp;
//    lp.show();

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
////    insertSql = R"(INSERT INTO user_message(`friendship_id`,`content`) VALUES(3,"this is test2");)"; /    if(lcon.update(insertSql)){ / LOG("success insert into val") /    }        LOG("success insert into val")
//    }

    // 注册类型到QML，如果需要的话
    // engine.rootContext()->setContextProperty("mySingleton", &mySingleton);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // //定义后端对象
    // LoginBackEnd loginBackEnd;
    // //将后端对象注册到QML中
    // engine.rootContext()->setContextProperty("loginBackEnd", &loginBackEnd);
    const QUrl url(QStringLiteral("qrc:/App.qml"));

    engine.load(url);
    return app.exec();
}
