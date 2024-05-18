# 部署项目
**本项目分为两个部分：客户端(src)与服务端(server)**

本项目部署缺少的依赖可以参考CMakeLists.txt即src/CMakeLists.txt、server/CMakeLists.txt

## 服务端部署
**服务端确保CMake版本>= 3.x.x;编译套件支持C++11及以上版本**

```CMakeLists
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(CURL REQUIRED)
```
- 项目使用的Protobuf版本为21.12：如果改换版本需要根据.proto文件重新生成protofile目录下的*.cc及*.h文件
- CURL版本为8.7.1

⚠ find_package的使用需要依赖提供 `*.cmake` 文件，也可以自行更改 CMakeLists 指定好库文件目录及头文件目录

## 客户端部署
**客户端确保Qt版本为6.x.x；项目当前V0.0.1编译套件使用 `msvc`**

⚠ 注意src/CMakeLists.txt需要修改，将其改为Qt msvc编译套件的绝对路径；
```CMakeLists
set(CMAKE_PREFIX_PATH "D:/QT/6.5.3/msvc2019_64")
```

# 技术部分

## ! Protobuf 做序列化网络传输
- 可与其他高级语言做兼容、扩展
- 其本质是二进制代码，服务器和客户端对于信息处理更快

## 服务端
- 使用 epoll 主从模式 利用线程池做并发处理
- 对于MySQL数据库连接和请求使用数据库连接池处理

## 客户端

** V1.x.x 针对于windows平台做扩展，后续尝试移植到Linux**
- 使用QT6进行客户端搭建、CMake做项目管理
- 使用QT Widget 作为Ui框架，核心内容大部分使用纯cpp及利用lib*库
- 使用SQlite3 做客户端数据持久化

## 数据库
- Mysql作为云端的存储数据库
- SQLite作为本地的聊天记录存储数据库

## 特别感谢
本项目的开发离不开社区的开源维护者，特此感谢：
|依赖|版本|备注|
|---|---|---|
|Protobuf|21.12|https://github.com/protocolbuffers/protobuf|
|curl|8.7.1|https://github.com/curl/curl|
|MySQL|8.0.20|https://www.mysql.com/cn/|
|Json|3.11.3|https://github.com/nlohmann/json|
|Qt|6.5.3|https://www.qt.io/zh-cn/|

# 功能部分

## 跨域通信
- 使用C/S架构：客户端通过Protobuf封装业务DTO；服务端业务驱动完成数据处理
- 聊天信息的备份：聊天信息在服务端通过MySQL持久化；在本地通过SQLite3持久化

### 点对点模式
- 即好友聊天，通过筛选唯一的ID值来添加好友进行聊天

### 点对面模式
- 即群聊模式，通过筛选唯一的群ID值来进行加入群聊进行聊天

## 文件上传/下载（暂未实现）
- 用户上传文件至服务器/用户从服务端下载文件

## 流媒体传输（暂定）
- 实时音视频通话

## 接入GPT（暂定）
- 通过构造json请求报文（需要proxy配置）
