#include <QApplication>
#include <QDebug>
#include <iostream>

#include "protofile/msgPacket.pb.h"
#include "page/base-arch/arch-page/ArchPage.h"
#include "yaml-cpp/yaml.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    ArchPage ap;
    ap.show();
    Msg::MsgPacket ms;
    ms.set_content("i am lihua!");
    ms.set_date_time("2024/2/21 10:21");
    ms.set_send_ssid("100001");
    ms.set_recv_ssid("100002");
    std::string output;
    ms.SerializeToString(&output);
    qDebug() << QString::fromStdString(output);
    return QApplication::exec();
}
