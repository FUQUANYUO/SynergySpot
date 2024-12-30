#!/bin/bash
# 停止脚本执行，如果 make 或 ./../out/Server 命令失败
set -e

if [ ! -d "./server/cmake-build" ]; then
    mkdir ./server/cmake-build && cd ./server/cmake-build
else
    cd ./server/cmake-build
fi

cmake ../..

# 解压LinuxLib
cd ../../src/lib-common/lib/Linux

# 初始化解压，若已经初始化则不解压及init mysql
if [ $(ls -1 | grep -E "LinuxLib-ubuntu\.tar\.gz|LinuxLib-centos\.tar\.gz" | wc -l) -eq 2 ] && [ $(ls -1 | grep -vE "LinuxLib-ubuntu\.tar\.gz|LinuxLib-centos\.tar\.gz" | wc -l) -eq 0 ]; then
  echo "lack of LinuxLib, uncompressing from LinuxLib-ubuntu.tar.gz"
  tar -xzvf LinuxLib-ubuntu.tar.gz || { echo "Failed to uncompress LinuxLib-ubuntu.tar.gz"; exit 1; }

# init mysql
  service mysql start
  service mysql status
  echo "------------------ begin to do init mysql ------------------"
  mysql -uroot -pSS123456 < ../../doc/数据库/sys_init.sql
  mysql -uroot -pSS123456 < ../../doc/数据库/synergy_spot.sql
  mysql -uroot -pSS123456 < ../../doc/数据库/synergy_spot_init.sql
  echo "------------------ init done ------------------"
else
  echo "The current directory does not contain exactly two files, skipping uncompress and init mysql"
fi

cd ../../../../server/cmake-build

# 执行 make 命令
echo "------------------ begin to do make ------------------"
make
echo "------------------ make done ------------------"


# 执行编译后的程序
./../out/Server
