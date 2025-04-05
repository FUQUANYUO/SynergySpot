<h1 align="center">
  SynergySpot
</h1>
<p align="center">
  SynergySpot 是一款以 QQ9 为参考并精心打造的聊天软件，力求提供丰富的功能和完善的用户体验。
</p>
<p align="center">
  中文名 “灵犀一点”，源自诗句"心有灵犀一点通"，形容即时的心灵沟通。
</p>


# 部署项目
**本项目分为两个部分：客户端(src)与服务端(server)**

本项目部署缺少的依赖可以参考CMakeLists.txt即src/CMakeLists.txt、server/CMakeLists.txt

## 服务端部署
**服务端确保在Linux环境下且CMake版本>= 3.x.x;编译套件支持C++11及以上版本**

```CMakeLists
set(CMAKE_PREFIX_PATH "/root/grpc_install")

# you need install grpc
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(absl REQUIRED)
```
- 服务端目前开放测试环境为 `ubuntu 22.04` 且将基于该版本的所有库文件放于 `/src/lib-common/lib/Linux` 下，也可使用 `docker-compose` 进行一键部署 (推荐)
- 使用在项目下的 `.gitmodule` 文件可以配置到对应的 `grpc` 版本以及对应的 `protobuf` 版本，通过官方示例教程可以进行编译得到 `grpc_install` 文件夹
- 项目使用的Protobuf版本为`3.19.5`：如果改换版本需要根据.proto文件重新生成protofile目录下的*.cc及*.h文件
- CURL版本为8.7.1

⚠ find_package的使用需要依赖提供 `*.cmake` 文件，也可以自行更改 CMakeLists 指定好库文件目录及头文件目录

## 客户端部署
**客户端确保Qt版本为6.x.x；项目当前V0.0.1编译套件使用 `msvc`**

⚠ 注意src/CMakeLists.txt需要修改，将其改为Qt msvc编译套件的绝对路径；
```CMakeLists
set(CMAKE_PREFIX_PATH "D:/QT/6.5.3/msvc2019_64")
```
- 若存在多个Qt版本环境，建议使用cmake前缀指定路径
- ⚠ 客户端需要在IDE中配置两个宏 `SS_CLINET` 以及 `SS_CLIENT_WIN` (后续开放SS_CLIENT_LINUX) ⚠
- 若编译后出现运行时闪退，可以使用 debug 查看缺少的 dll，一般情况下 cmake 会将位于 `/src/lib-common/lib/Win` 下的所有 dll 放于 `/src/out/{CMAKE_BUILD_TYPE}` 下，若出现崩溃可以尝试重新生成 cmake 缓存或者直接手动复制 dll
- 若出现编译过程中的 `.qrc` 文件的相关报错，可能是因为 cmake 在写入 `.qrc` 时存在并行冲突，删除任意 `.qrc` 中的内容重新生成即可解决
## 配置文件
**上述的依赖解决后，更改src/conf/clientInfo-example.yaml以及src/conf/serverInfo-example.yaml中的内容并更改文件名为clientInfo.yaml和serverInfo.yaml**
需要更改的内容：
```yaml
# clientInfo.yaml
server-host-info:
 ip: [服务端部署地址]
 port: [服务端端口]
open-webui-info:
 url: [部署的`open webui`项目所在url]

# serverInfo.yaml
mysql-info:
 ip: [服务端MySQL服务IP]
 port: [服务端MySQL端口]
 userName: [MySQL用户名]
 password: [MySQL用户密码]

host-info:
 listenPort: [服务端监听端口]
email-info:
 sendAccount: [发送邮件邮箱]
 emailServer: smtp://smtp.qq.com:587
 accessToken: [邮箱授权码]
realtime-info:
 grpcIP: "0.0.0.0"
 grpcPort: [grpc服务端端口]
 maxRetryCount: 5
 fileSavePath: [服务端文件存储路径]
```

**执行SQL脚本**
在MySQL服务中执行doc/数据库/synergy_spot.sql 及 doc/数据库/synergy_spot_init.sql

# 技术部分

## ! Protobuf 做序列化网络传输
- 可与其他高级语言做兼容、扩展
- 其本质是二进制代码，服务器和客户端对于信息处理更快

## 服务端
- grpc 服务端作为 epoll 服务端的子进程，处理文件传输，心跳检测相关性能消耗较大的业务
- 使用 epoll 边缘触发，主从模式 利用线程池做并发处理
- 对于MySQL数据库连接和请求使用数据库连接池处理

## 客户端

**V1.x.x 针对于windows平台做扩展，后续尝试移植到Linux**
- 使用QT6进行客户端搭建、CMake做项目管理
- 使用 ElaWidgetTool 作为Ui框架，核心内容大部分使用纯cpp及利用lib*库
- 使用 SQlite3 做客户端数据持久化

## 数据库
- Mysql作为云端的存储数据库
- SQLite作为本地的聊天记录存储数据库

## 特别感谢
本项目的开发离不开社区的开源维护者，特此感谢：
|依赖|版本|备注|
|---|---|---|
|ElaWidgetTools|-|https://github.com/Liniyous/ElaWidgetTools|
|GRPC|V1.46.5|https://github.com/grpc/grpc|
|Protobuf|V3.19.5|https://github.com/protocolbuffers/protobuf|
|sqlcipher|V4.6.1|https://github.com/sqlcipher/sqlcipher|
|curl|V8.7.1|https://github.com/curl/curl|
|MySQL|V8.0.20|https://www.mysql.com/cn/|
|Json|V3.11.3|https://github.com/nlohmann/json|
|Qt|V6.5.3|https://www.qt.io/zh-cn/|

# 功能部分

## 跨域通信
- 使用C/S架构：客户端通过Protobuf封装业务DTO；服务端业务驱动完成数据处理
- 聊天信息的备份：聊天信息在服务端通过MySQL持久化；在本地通过SQLite3持久化

### 点对点模式
- 即好友聊天，通过筛选唯一的ID值来添加好友进行聊天

### 点对面模式
- 即群聊模式，通过筛选唯一的群ID值来进行加入群聊进行聊天

## 文件上传/下载
- 用户上传文件至服务器/用户从服务端下载文件

## 流媒体传输（V0.0.1b版本开始采用腾讯云的 TRTC 音视频接口）
- 实时音视频通话

## 接入GPT（V0.0.1b版本采用使用 QtWebEngine 渲染部署的 open-webui 项目）
- 配置相关的API信息
