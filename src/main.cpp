#include <QApplication>
#include <QDebug>
#include <iostream>

#include "protofile/msgPacket.pb.h"
#include "page/base-arch/arch-page/ArchPage.h"
#include "yaml-cpp/yaml.h"
#include "db-pool/ConnectionPool.h"

#include "mysql.h"
#include "yaml-cpp/yaml.h"

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
    ArchPage ap;
    ap.show();
    Msg::MsgPacket ms;
// 数据库测试
// ConnectionPool::getConnectPool()->getConnection()->update("insert into user_private_info(`ssid`,`status`,`password`) values('10000001',0,'Fu159753')");
    GetQueryRes(ConnectionPool::getConnectPool()->getConnection()->query("SELECT `ssid`,`status`,`password` FROM user_private_info"),{0,1,2});

    
//    protobuf 测试
//    ms.set_content("i am lihua!");
//    ms.set_date_time("2024/2/21 10:21");
//    ms.set_send_ssid("100001");
//    ms.set_recv_ssid("100002");
//    std::string output;
//    ms.SerializeToString(&output);
//    qDebug() << QString::fromStdString(output);
    return QApplication::exec();
}
