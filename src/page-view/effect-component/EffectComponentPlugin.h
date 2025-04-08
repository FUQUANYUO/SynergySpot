//
// Created by FU-QAQ on 2024/12/6.
//

#ifndef SYNERGYSPOT_EffectComponentPlugin_H
#define SYNERGYSPOT_EffectComponentPlugin_H

#include "define.h"
#include "opencv2/opencv.hpp"
#include <QImage>

#ifdef SS_PLATFORM_WINDOWS
    #ifdef SS_EFFECT_COMPONENT_EXPORTS
        #define SS_API __declspec(dllexport)
    #else
        #define SS_API __declspec(dllimport)
    #endif
#elif defined SS_PLATFORM_LINUX
    #ifdef SS_EFFECT_COMPONENT_EXPORTS
        #define SS_API __attribute__((visibility("default")))
    #else
        #define SS_API
    #endif
#endif

#define BK_PLUGIN_NAME "BackgroundStrategyPlugin"

class SS_API BackgroundStrategyPlugin : public SS_Plugin{
public:
    ~BackgroundStrategyPlugin() override = default;
    virtual bool initialize(const QString& videoSource) = 0;
    virtual void updateFrame() = 0;
    virtual void releaseCamera() = 0;
    virtual QImage getCurrentFrame() const = 0;
    virtual bool isFinished() const = 0;
    virtual void reset() = 0;
};

extern "C" SS_API BackgroundStrategyPlugin* createCVProVideoStrategyPlugin();

#endif
