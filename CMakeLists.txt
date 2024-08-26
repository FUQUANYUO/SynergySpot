cmake_minimum_required(VERSION 3.20)

project(SynergySpot VERSION 1.0)

if(WIN32)
    add_subdirectory(src)
else()
    add_subdirectory(server)
endif()