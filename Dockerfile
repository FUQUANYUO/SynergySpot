FROM ubuntu:22.04

# 使用阿里云的 Ubuntu 镜像源
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list \
  && sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list

# 安装必要的软件包
RUN apt-get update && apt-get install -y \
    software-properties-common \
    && add-apt-repository ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    && apt-get install -y \
    gcc-11 g++-11 \
    gdb \
    cmake \
    mysql-server

# 设置默认的 gcc 和 g++ 版本为 11
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 60 --slave /usr/bin/g++ g++ /usr/bin/g++-11

# 检查安装的版本
RUN gcc --version
RUN g++ --version
RUN gdb --version
RUN cmake --version
RUN mysql --version

# 配置MySQL root密码
RUN echo "mysql-server mysql-server/root_password password SS123456" | debconf-set-selections \
 && echo "mysql-server mysql-server/root_password_again password SS123456" | debconf-set-selections

# 设置工作目录
WORKDIR /SynergySpotWorkSpace

# 复制源码
COPY . /SynergySpotWorkSpace

# 编译 server 项目源码
RUN cd /SynergySpotWorkSpace \
    && cd ./server/cmake-build \
    && cmake ..

RUN chmod 777 ./start.sh

CMD ["./start.sh"]
