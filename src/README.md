# 客户端架构体系

# 1、客户端技术栈

- Qt6
	- QWiget                               **界面开发**              
	- QThread                            **子线程并发**           
	- QListView&QTreeView    **M/V模式委托绘制** 
	- QTcpSocket                       **客户端网络传输和接收**
- C++11 
- Protobuf                                   **业务DTO封装** 
- CMake                                       **项目构建管理** 
- SQlite3                                      **本地消息存储**
- Yaml                                          **配置文件**        
- 数据库连接池



# 2、客户端功能点添加

> 客户端基于Qt的信号与槽机制来完成各项后端业务，其中与客户端与服务端的通信是在子线程中进行的，我们进行跨线程通信需要多次的转发信号来完成业务通信；



> 首先注意本项目中的命名规则（切不可使用拼音）：
>
> - 对于**文件夹命名**为小写英文词组分隔使用 '-'；
>
> - 对于**头文件和附属源文件**遵循词组开头大写的原则并且头文件与附属源文件命名一致；
> - 对于**Page文件夹下**（也就是前端的组件类添加）文件夹命名遵循 xxx-page 文件命名遵循 XxxPage
> - 对于**信号**统一使用词组全大写，以 '-' 分隔的方式；
> - 对于**槽函数**使用词组首字母大写；
> - 对于**信号和槽函数中有关protobuf的变量**命名统一为
>   - 1、protobuf生成的字符串为 outXXXdto
>   - 2、protobuf待解析的字符串为 rawXXXdto 



>  关于注释：
>
> 只需要解释注释下属代码段在整个业务中的作用；
>
> 注释不要加标点，一句话概括；
>
> 注释不需要写小作文，需要有详细阐明请用多行注释；
>
> 注释最好在  '//' 空一格完善 比如: // 这是一个注释 



## Core 后端功能点添加

> 首先判断功能点类型：
>
> 如果是基础类型也就是该功能点缺少会导致程序崩溃，令用户体验不完善的则在base文件夹下。
>
> 如果是额外的类型，请自行判断创建文件夹。



![BToA](..\doc\Static\bToA.png)



### `BusinessListen.h & BusinessListen.cpp`  



>  这个类中可以说是一个仿路由，客户端的所有请求通过它转发给服务端；服务端的所有请求通过它解析并触发客户端注册的槽函数。



> 既然作为路由我们在添加业务时就需要在这个头文件就中完成注册操作，下面我们以添加一个业务来作为示例：



业务描述：客户端向服务端打招呼，服务端收到招呼后进行回应；



> 分析业务，其作为示例我们暂时定为base功能

我们在 base 文件夹下建立如下结构：

- hello
  - Hello.cpp
  - Hello.h

```Cpp
#ifndef SYNERGYSPOT_HELLO_H
#define SYNERGYSPOT_HELLO_H

#include <QObject>

class Hello : public QObject{
    Q_OBJECT
public:
    Hello() = default;
    ~Hello() = default;
};


#endif//SYNERGYSPOT_HELLO_H
```



> 随后我们需要去 BusinessLisent 注册这个业务信号，即HELLO_TO_SERVER



> BusinessLisent.h 片段

```Cpp
class BusinessListen : public QObject{
    Q_OBJECT
public:
    BusinessListen();
    ~BusinessListen();
signals:
    // 建立与服务器的连接
    void CONTOSER();

    // 邮箱验证
    void REQUEST_EMAILCODE(const std::string outEdto);
    void GET_EMAILCODE(const std::string rawEdto);

    // 登录验证信号
    void VERIFY_ACCOUNT(const std::string outLdto);
    void LAND_SUCCESS();
    void LAND_FAIL();

    // 接收消息
    void FORWARD_MSG(const std::string outFmdto);
    void RECV_MSG(const std::string rawFmdto);

    // 获取联系人列表
    void REQUEST_CONTACTLIST(const std::string outGfdto);
    void GET_CONTACTLIST(const std::string rawGfdto);
    
    // 向服务端打招呼（这里是我们的示例）
    void HELLO_TO_SERVER(const std::string outHSdto);
    void GET_SERVER_RESPONSE(const std::string rawHSdto);
private:
    DoBusiness::AllocBusiness * allocBusiness;
    QThread * _t;
};
```



>  上述代码段信息量非常多，我们逐一分析：

- BusinessLisent 私有变量有一个子线程和一个组合类型（这里的子线程就是为了这个组合类型开辟的后续我们会讨论到）

- 其他的地方我们不做关注，只看我们的向服务端打招呼这个类型，我们传入是一个字符串类型，其实就是protobuf的消息体字节序成的那个 out 字符串（protobuf我们随后聊到）

- 在 `HELLO_TO_SERVER` 信号后有一个`GET_SERVER_RESPONSE`信号，这个就是前面埋下的铺垫（子线程需要通过多次信号发送来完成跨线程通信）

  

  **当子线程获取到服务端的业务回传后，会发送这个信号，供给在前端的槽函数触发**。



> 上述的分析只需要记得业务想要与服务端进行通信，则需要先在BusinessLisent中注册好信号，并设置好回传信号，以便接收服务端回传信息。



> 上述处理是为了下面谈到的 AllocBusiness 这个类型



### `AllocBusiness`

> 下述为节选代码

```Cpp
// 分配业务
    class AllocBusiness : public QObject{
        Q_OBJECT
    public:
        explicit AllocBusiness();
        ~AllocBusiness();
        void run();
    public slots:
        // 断开连接
        void DisConnFromSer();
        // 连接服务端
        void ConToSer();
    signals:
        // 断开连接
        void DISCONNECTFROMSER();
        // 未读业务
        void BUSINESSTODO();
    private:
        ClientConServer * _ccon;
    };
```



> 我们可以看到在这个类型中是有一个QTcpSocket的封装成员的（ClientConServer）

> 而这个`AllocBusiness`的变量会转移到子线程中，也就是我们在子线程中去做了网络的通信和发送。



我们结合 `BusinessLisent` 和 `AllocBusiness`一起来看，在`BusinessLisent`的构造函数中（节选）：



```Cpp
BusinessListen::BusinessListen() {
    allocBusiness = new DoBusiness::AllocBusiness();
    _t = new QThread(allocBusiness);

	// 有从服务端来的未读业务则调用其分配函数
    connect(allocBusiness,&DoBusiness::AllocBusiness::BUSINESSTODO,allocBusiness,&DoBusiness::AllocBusiness::run);
    
    // 业务通过子线程发送给服务端
    connect(this,&BusinessListen::VERIFY_ACCOUNT,allocBusiness,&DoBusiness::AllocBusiness::VerifyAcc);

    // 转发子线程的信号
    connect(allocBusiness,&DoBusiness::AllocBusiness::LAND_SUCCESS,this,[=](){
        emit LAND_SUCCESS();
    });
    connect(allocBusiness,&DoBusiness::AllocBusiness::LAND_FAIL,this,[=](){
        emit LAND_FAIL();
    });

    // 开启事务监听
    _t->start();

}
```



> 上述的代码片段蕴含了两个信息：
>
> - 一个是BusinessLisent的信号会触发AllocBusiness的槽函数
> - 一个是AllocBusiness的信号也会触发BusinessLisent的槽函数



其他的代码部分请大家去看看源文件中的实际例子，下面我们说回我们的 Hello 操作：

1. 在AllocBusiness类中注册映射BusinessLisent的槽函数，以及网络回传后发给BusinessLisent的信号



```Cpp
namespace DoBusiness{
    // 分配业务
    class AllocBusiness : public QObject{
        Q_OBJECT
    public:
        explicit AllocBusiness();
        ~AllocBusiness();
        void run();
    public slots:
		// HELLO
        void hello(const std::string& outHSdto);
    signals:
        // HELLO
		void GET_SERVER_RESPONSE(const std::string& rawHSdto);
    private:
        ClientConServer * _ccon;
    };
	// HELLO 
    DO_CALLBACK_CLASS(Hello,helloToServer,void)
}
```



> DO_CALLBACK_CLASS 是一个遵循命名模式的宏具体如下：

```Cpp
// 回调函数宏
#define DO_CALLBACK_CLASS(__CLASS_NAME__,__FUNC_NAME__,__FUNC_RETURN_TYPE__)    \
class __CLASS_NAME__ : public QObject{                                          \
public:                                                                         \
    explicit __CLASS_NAME__(QObject * parent,std::string dto = "");             \
    void __FUNC_NAME__();                                                       \
    ~__CLASS_NAME__() = default;                                                \
private:                                                                        \
    std::string dto;                                                            \
};
```



> 回到BusinessLisent构造函数：

```Cpp
BusinessListen::BusinessListen() {
    allocBusiness = new DoBusiness::AllocBusiness();
    _t = new QThread(allocBusiness);

	// 有从服务端来的未读业务则调用其分配函数
    connect(allocBusiness,&DoBusiness::AllocBusiness::BUSINESSTODO,allocBusiness,&DoBusiness::AllocBusiness::run);
    
    // 业务通过子线程发送给服务端
    connect(this,&BusinessListen::HELLO,allocBusiness,&DoBusiness::AllocBusiness::hello);

    // 转发子线程的信号
    connect(allocBusiness,&DoBusiness::AllocBusiness::GET_SERVER_RESPONSE,this,[=](const std::string& rawHSdto){
        emit GET_SERVER_RESPONSE(std::move(rawHSdto));
    });

    // 开启事务监听
    _t->start();
}
```



> 最后我们来看看 AllocBusiness 在子线程中做的事：

```Cpp
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
    }else if(type == SSDTO::HELLO){
        DoBusiness::Hello h(this,dto.toStdString());
        h.helloToServer();
    }
    LOG("recv some business to do from server : " + std::to_string(type))
}
```



> 其就是每当触发了QTcpSocket的readed信号后，就在这个run()子线程函数中去读取服务端到客户端的业务类型，并根据业务类型做相应的 `命令`

> 上述提及的命令模式就是置于此处的增加代码可读性和减少run函数中的代码量



至此客户端的Hello配置完成。



