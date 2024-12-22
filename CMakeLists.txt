cmake_minimum_required(VERSION 3.20)

project(SynergySpot VERSION 1.0)

if(SS_CLIENT)
    add_subdirectory(src)
else()
    add_subdirectory(server)
endif()