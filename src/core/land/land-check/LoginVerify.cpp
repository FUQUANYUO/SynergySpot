//
// Created by FU-QAQ on 2024/2/25.
//
#include "help.h"
#include "LoginVerify.h"

// protobuf 登录业务
#include "login/LoginDTO.pb.h"

// TODO email verify and get ip verify

void LoginVerify::verifyInServer(std::string ssid, std::string password) {
    if(_ccons.getQSocket()->state() != QAbstractSocket::ConnectedState){
        // 建立连接
        _ccons.ConnToSer();
    }

    // 验证 ssid 对应的 password
    SSDTO::Login_DTO ldto;
    ldto.set_type(SSDTO::Business_Type::LOGIN);
    ldto.set_ssid(ssid);
    ldto.set_password(password);
    ldto.set_is_pass(false);
    ldto.set_ip("");
    ldto.set_email("");
    std::string output;
    ldto.SerializeToString(&output);

    // 发送检验包
    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream << static_cast<quint32>(output.size());
    stream << static_cast<quint8>(ldto.type());
    packet.append(output.c_str(),output.size());
    _ccons.getQSocket()->write(packet);
    _ccons.getQSocket()->waitForBytesWritten();
    LOG("dto has been send to server...")

    connect(_ccons.getQSocket(),&QTcpSocket::connected,this,[=](){
        LOG("connect success!")
    });

    // 接收服务端传回结果
    connect(_ccons.getQSocket(),&QTcpSocket::readyRead,this,[=](){
        LOG("recv server verify [" << ssid << "] res")
        SSDTO::Login_DTO ldto;
        // 读取4字节
        QByteArray sizeData = _ccons.getQSocket()->read(4);
        QDataStream sizeStream(&sizeData,QIODevice::ReadOnly);
        int dataSize = 0;
        sizeStream >> dataSize;

        // 读取dto
        QString resDtoStr(_ccons.getQSocket()->read(dataSize));
        ldto.ParseFromString(resDtoStr.toStdString());
        if(ldto.is_pass())
            emit LAND_SUCCESS();
        else
            emit LAND_FAIL();
    });
}
