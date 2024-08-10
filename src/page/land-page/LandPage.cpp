//
// Created by FU-QAQ on 2024/2/10.
//

// You may need to build the project (run Qt uic code generator) to get "ui_LandPage.h" resolved
#include "LandPage.h"
#include "ui_LandPage.h"

#include "register-page/RegisterPage.h"
#include "base-arch/arch-page/ArchPage.h"

//----------- middle software -----------//
// mysql connection pool
#include "db-pool/ConnectionPool.h"
// parse yaml file
#include "yaml-cpp/yaml.h"

// client info yaml file path
#ifdef WIN32
std::string yamlPath = "../../conf/clientInfo.yaml";
#else
std::string yamlPath = "../conf/clientInfo.yaml";
#endif

std::string CurSSID;
std::string CurSSname;

LandPage::LandPage(QWidget *parent) : QMainWindow(parent), ui(new Ui::LandPage) {
    ui->setupUi(this);
    bl = new BusinessListen();
    lver = new LoginVerify(bl);
    // 注册账号
    connect(ui->registerBtn,&QPushButton::clicked,this,[&](){
        emit bl->CONTOSER();
        auto * rgPage = new RegisterPage(this);
        rgPage->show();
    });
    // 登录
    connect(ui->loginBtn,&QPushButton::clicked,this,[&](){
        QString inputSSID = ui->ssidInput->text();
        QString inputPassword = ui->passwordInput->text();

        if(!inputSSID.isEmpty() && !inputPassword.isEmpty()){
            // 连接服务端
            emit bl->CONTOSER();
#ifdef LOC_TEST
            // 本地检验
            char sqlStr[100];
            sprintf(sqlStr,"SELECT `password` from user_private_info WHERE `ssid`='%s'",
                    inputSSID.toStdString().c_str());
            MYSQL_RES * mRes = ConnectionPool::getConnectPool()->getConnection()->query(sqlStr);
            if(mRes == nullptr) {
                cout <<  __FILE__ << " | " << __LINE__ << " | " << __TIME__ << " | " << "verify error" << endl;
                emit lver.LAND_FAIL();
            }else{
                MYSQL_ROW row;
                while((row = mysql_fetch_row(mRes))){
                    string comS(row[0]);
                    if(comS == inputPassword.toStdString()){
                        cout << "verify success" << endl;
                        emit lver.LAND_SUCCESS();
                    }else{
                        cout <<  __FILE__ << " | " << __LINE__ << " | " << __TIME__ << " | " << "verify error" << endl;
                        emit lver.LAND_FAIL();
                    }
                }
                mysql_free_result(mRes);
            }
#else
            // 云端检验
            CurSSID = ui->ssidInput->text().toStdString();
            lver->verifyInServer(inputSSID.toLocal8Bit().data(),inputPassword.toLocal8Bit().data());
#endif
        }
    });

    // 数据库检验通过
    connect(bl,&BusinessListen::LAND_SUCCESS,this,[=](){
        // 生成框架界面
        auto * ap = new ArchPage(bl);
        ap->show();
        // 关闭登录页面
        this->close();
    });
}

LandPage::~LandPage() {
    delete ui;
}
BusinessListen *LandPage::getBusinessObj() {
    return bl;
}
