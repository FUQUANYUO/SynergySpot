#!/bin/sh
# 停止脚本执行，如果 make 或 ./../out/Server 命令失败
set -e


if [[ ! -d "./server/cmake-build" ]]; then
    mkdir ./server/cmake-build && cd ./server/cmake-build
else
    cd ./server/cmake-build
fi

cmake ..

# 执行 make 命令
make 

service mysql start

service mysql status
mysql -uroot -pSS123456 < ../../doc/数据库/sys_init.sql
mysql -uroot -pSS123456 < ../../doc/数据库/synergy_spot.sql
mysql -uroot -pSS123456 < ../../doc/数据库/synergy_spot_init.sql

# 执行编译后的程序
./../out/Server
