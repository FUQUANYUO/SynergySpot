//
// Created by FU-QAQ on 2024/3/1.
//
#include <QObject>
#include <QtEndian>

#include "BusinessListen.h"
#include "DTO.pb.h"
#include "help.h"

#include "land/land-check/LoginVerify.h"


extern std::string CurSSID;

BusinessListen::BusinessListen() {
    allocBusiness = new DoBusiness::AllocBusiness();
    _t = new QThread(allocBusiness);

    /* 需要操作子线程的socket需要通过信号与槽机制，最好是在ConToSer这个槽函数中注册这些事件 */

    // 建立连接
    connect(this,&BusinessListen::CONTOSER,allocBusiness,&DoBusiness::AllocBusiness::ConToSer);

    // 有从服务端来的未读业务则调用其分配函数
    connect(allocBusiness,&DoBusiness::AllocBusiness::BUSINESSTODO,allocBusiness,&DoBusiness::AllocBusiness::run);

    // 业务通过子线程发送给服务端
    connect(this,&BusinessListen::VERIFY_ACCOUNT,allocBusiness,&DoBusiness::AllocBusiness::VerifyAcc);
    connect(this,&BusinessListen::FORWARD_MSG,allocBusiness,&DoBusiness::AllocBusiness::ForwardBySer);
    connect(this,&BusinessListen::REQUEST_CONTACTLIST,allocBusiness,&DoBusiness::AllocBusiness::QueryContactList);

    // 转发子线程的信号
    connect(allocBusiness,&DoBusiness::AllocBusiness::LAND_SUCCESS,this,[=](){
        emit LAND_SUCCESS();
    });
    connect(allocBusiness,&DoBusiness::AllocBusiness::LAND_FAIL,this,[=](){
        emit LAND_FAIL();
    });
    connect(allocBusiness,&DoBusiness::AllocBusiness::RECV_MSG,this,[=](std::string rawFmdto){
        emit RECV_MSG(std::move(rawFmdto));
    });
    connect(allocBusiness,&DoBusiness::AllocBusiness::GET_CONTACTLIST,this,[=](std::string rawGfdto){
        emit GET_CONTACTLIST(std::move(rawGfdto));
    });

    // 开启事务监听
    _t->start();
}
BusinessListen::~BusinessListen() {
    emit allocBusiness->DISCONNECTFROMSER();
    _t->requestInterruption();
    _t->wait(100);
    _t->quit();
    _t->deleteLater();
}

DoBusiness::RecvMsgTask::RecvMsgTask(QObject * parent,std::string dto) : QObject(parent), dto(std::move(dto)){
}

void DoBusiness::RecvMsgTask::forwardMsg() {
    dynamic_cast<AllocBusiness*>(parent())->RECV_MSG(dto);
}

DoBusiness::RecvVerifyRes::RecvVerifyRes(QObject * parent,std::string dto) : QObject(parent), dto(std::move(dto)){
}

void DoBusiness::RecvVerifyRes::parseVerifyRes() {
    LOG("recv server verify [" << CurSSID << "] res")
    SSDTO::Login_DTO ldto;
    ldto.ParseFromString(dto);
    if(ldto.is_pass())
        emit dynamic_cast<AllocBusiness*>(parent())->LAND_SUCCESS();
    else
        emit dynamic_cast<AllocBusiness*>(parent())->LAND_FAIL();
}


DoBusiness::AllocBusiness::AllocBusiness() {
    _ccon = new ClientConServer();
}

void DoBusiness::AllocBusiness::run() {
    QByteArray array = _ccon->getQSocket()->read(4);
    int msgSize = 0;
    QDataStream sizeStream(&array,QIODevice::ReadOnly);
    sizeStream >> msgSize;

    array = _ccon->getQSocket()->read(4);
    QDataStream typeStream(&array,QIODevice::ReadOnly);
    SSDTO::Business_Type type;
    typeStream >> type;

    QString dto = _ccon->getQSocket()->read(msgSize);
    if(type == SSDTO::LOGIN){
        DoBusiness::RecvVerifyRes vres(this,dto.toStdString());
        vres.parseVerifyRes();
    }else if(type == SSDTO::ENROLL){
        DoBusiness::RecvEnrollRes eres();
    }else if(type == SSDTO::FOWARD_MSG){
        DoBusiness::RecvMsgTask mtask(this,dto.toStdString());
        mtask.forwardMsg();
    }else if(type == SSDTO::GET_CONTACTLIST){
        DoBusiness::GetContactList gcl(this,dto.toStdString());
        gcl.getContactList();
    }
    LOG("recv some business to do from server")
}
void DoBusiness::AllocBusiness::VerifyAcc(const std::string& outLdto) {
    // 发送检验包
    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream << static_cast<quint32>(outLdto.size());
    stream << static_cast<quint8>(SSDTO::LOGIN);
    packet.append(outLdto.c_str(),outLdto.size());

    _ccon->getQSocket()->write(packet);
    _ccon->getQSocket()->waitForBytesWritten();
    LOG("dto has been send to server...")
}

void DoBusiness::AllocBusiness::ForwardBySer(const std::string& outFmdto) {
    // 发送转发包
    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream << static_cast<quint32>(outFmdto.size());
    stream << static_cast<quint8>(SSDTO::FOWARD_MSG);
    packet.append(outFmdto.c_str(),outFmdto.size());
    _ccon->getQSocket()->write(packet);
    _ccon->getQSocket()->waitForBytesWritten();
    LOG("msg_forward dto has been send to server...")
}

DoBusiness::AllocBusiness::~AllocBusiness() {
    _ccon->getQSocket()->disconnectFromHost();
    _ccon->~ClientConServer();
}

void DoBusiness::AllocBusiness::ConToSer() {
    // 连接服务器
    if(_ccon->getQSocket()->state() != QAbstractSocket::ConnectedState){
        // 建立连接
        _ccon->ConnToSer();
    }
    connect(_ccon->getQSocket(),&QTcpSocket::connected,[=](){
        LOG("connect success")
    });

    // 未读业务发送信号
    connect(_ccon->getQSocket(),&QTcpSocket::readyRead,this,[&](){
        emit BUSINESSTODO();
    });

    // 断开信号
    connect(this,&AllocBusiness::DISCONNECTFROMSER,this,&AllocBusiness::DisConnFromSer);
}

void DoBusiness::AllocBusiness::DisConnFromSer() {
    // 发送离线通知
    std::string outDisdto;
    SSDTO::Disconnect_DTO ddto;
    ddto.set_ssid(CurSSID);
    ddto.set_ip("");
    ddto.set_type(SSDTO::DISCONNECT);
    ddto.SerializeToString(&outDisdto);

    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream << static_cast<quint32>(outDisdto.size());
    stream << static_cast<quint8>(SSDTO::DISCONNECT);
    packet.append(outDisdto.c_str(),outDisdto.size());
    _ccon->getQSocket()->write(packet);
    _ccon->getQSocket()->waitForBytesWritten();
    LOG("disconnect notice has been send to server...")
}
void DoBusiness::AllocBusiness::QueryContactList(const std::string &outGfdto) {
    // 发送检验包
    QByteArray packet;
    QDataStream stream(&packet,QIODevice::WriteOnly);
    stream << static_cast<quint32>(outGfdto.size());
    stream << static_cast<quint8>(SSDTO::GET_CONTACTLIST);
    packet.append(outGfdto.c_str(),outGfdto.size());

    _ccon->getQSocket()->write(packet);
    _ccon->getQSocket()->waitForBytesWritten();
    LOG("query contact list dto has been send to server...")
}

DoBusiness::GetContactList::GetContactList(QObject * parent,std::string dto) : QObject(parent), dto(std::move(dto)) {
}

void DoBusiness::GetContactList::getContactList() {
    emit dynamic_cast<AllocBusiness*>(parent())->GET_CONTACTLIST(dto);
}
