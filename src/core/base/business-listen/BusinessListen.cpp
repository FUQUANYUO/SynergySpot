//
// Created by FU-QAQ on 2024/3/1.
//
#include <QObject>
#include <QtEndian>

#include "BusinessListen.h"
#include "DTO.pb.h"
#include "help.h"

#include "land/land-check/LoginVerify.h"

// 发送包装宏
#define SEND_PACKAGE(__DTO_OBJ__,__DTO_TYPE__,__LOG__)      \
    QByteArray packet;                                      \
    QDataStream stream(&packet,QIODevice::WriteOnly);       \
    stream << static_cast<quint32>(__DTO_OBJ__.size());     \
    stream << static_cast<quint8>(__DTO_TYPE__);            \
    packet.append(__DTO_OBJ__.c_str(),__DTO_OBJ__.size());  \
    _ccon->getQSocket()->write(packet);                     \
    _ccon->getQSocket()->waitForBytesWritten();             \
    LOG(__LOG__)                                            \

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
    connect(this,&BusinessListen::REQUEST_EMAILCODE,allocBusiness,&DoBusiness::AllocBusiness::QueryEmailVerifyCode);

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
    connect(allocBusiness,&DoBusiness::AllocBusiness::GET_EMAILCODE,this,[=](std::string rawEdto){
        emit GET_EMAILCODE(std::move(rawEdto));
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

DoBusiness::GetContactList::GetContactList(QObject * parent,std::string dto) : QObject(parent), dto(std::move(dto)) {
}

void DoBusiness::GetContactList::getContactList() {
    emit dynamic_cast<AllocBusiness*>(parent())->GET_CONTACTLIST(dto);
}

DoBusiness::GetEmailCode::GetEmailCode(QObject *parent, std::string dto) : QObject(parent), dto(std::move(dto)){
}

void DoBusiness::GetEmailCode::getEmailCode() {
    emit dynamic_cast<AllocBusiness*>(parent())->GET_EMAILCODE(dto);
}

DoBusiness::AllocBusiness::AllocBusiness() {
    _ccon = new ClientConServer();
}

DoBusiness::AllocBusiness::~AllocBusiness() {
    _ccon->getQSocket()->disconnectFromHost();
    _ccon->~ClientConServer();
}

void DoBusiness::AllocBusiness::ConToSer() {
    // 连接服务器
    if(_ccon->getQSocket()->state() != QAbstractSocket::ConnectedState){
        // 建立连接
        _ccon->connToSer();
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

void DoBusiness::AllocBusiness::run() {
    QByteArray array = _ccon->getQSocket()->read(4);
    int msgSize = 0;
    QDataStream sizeStream(&array, QIODevice::ReadOnly);
    sizeStream >> msgSize;

    array = _ccon->getQSocket()->read(4);
    QDataStream typeStream(&array, QIODevice::ReadOnly);
    SSDTO::Business_Type type;
    typeStream >> type;

    QString dto = _ccon->getQSocket()->read(msgSize);
    if (type == SSDTO::LOGIN) {
        DoBusiness::RecvVerifyRes vres(this, dto.toStdString());
        vres.parseVerifyRes();
    } else if (type == SSDTO::ENROLL) {
        // DoBusiness::RecvEnrollRes eres();
    } else if(type == SSDTO::GET_EMAILCODE){
        DoBusiness::GetEmailCode gec(this,dto.toStdString());
        gec.getEmailCode();
    } else if(type == SSDTO::FOWARD_MSG){
        DoBusiness::RecvMsgTask mtask(this,dto.toStdString());
        mtask.forwardMsg();
    } else if(type == SSDTO::GET_CONTACTLIST){
        DoBusiness::GetContactList gcl(this,dto.toStdString());
        gcl.getContactList();
    }
    LOG("recv some business to do from server : " + std::to_string(type))
}

void DoBusiness::AllocBusiness::VerifyAcc(const std::string& outLdto) {
    // 发送检验包
    SEND_PACKAGE(outLdto,SSDTO::LOGIN,"dto has been send to server...")
}

void DoBusiness::AllocBusiness::ForwardBySer(const std::string& outFmdto) {
    // 发送转发包
    SEND_PACKAGE(outFmdto,SSDTO::FOWARD_MSG,"msg_forward dto has been send to server...")
}

void DoBusiness::AllocBusiness::DisConnFromSer() {
    // 发送离线通知
    std::string outDisdto;
    SSDTO::Disconnect_DTO ddto;
    ddto.set_ssid(CurSSID);
    ddto.set_ip("");
    ddto.set_type(SSDTO::DISCONNECT);
    ddto.SerializeToString(&outDisdto);

    SEND_PACKAGE(outDisdto,SSDTO::DISCONNECT,"disconnect notice has been send to server...")
}

void DoBusiness::AllocBusiness::QueryContactList(const std::string &outGfdto) {
    // 发送查询联系人列表请求
    SEND_PACKAGE(outGfdto,SSDTO::GET_CONTACTLIST,"query contact list dto has been send to server...")
}

void DoBusiness::AllocBusiness::QueryEmailVerifyCode(const std::string &outEdto) {
    // 发送请求邮箱验证码
    SEND_PACKAGE(outEdto,SSDTO::GET_EMAILCODE,"query email code dto has been send to server...")
}
