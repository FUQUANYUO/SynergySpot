//
// Created by FU-QAQ on 2024/2/25.
//

#ifndef SYNERGYSPOT_DEFINE_H
#define SYNERGYSPOT_DEFINE_H

#ifdef _WIN32
    #define SS_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define SS_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

#ifdef SS_PLATFORM_WINDOWS
    #define SS_API_OUT __declspec(dllexport)
    #define SS_API_IN __declspec(dllimport)
#elif defined(SS_PLATFORM_LINUX)
    #ifdef SS_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#else
    #error "Neither SS_PLATFORM_WINDOWS nor SS_PLATFORM_LINUX is defined"
#endif



#endif//SYNERGYSPOT_DEFINE_H
