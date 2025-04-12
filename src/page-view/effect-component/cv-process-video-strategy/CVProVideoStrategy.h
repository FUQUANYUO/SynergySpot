//
// Created by FU-QAQ on 2024/12/7.
//

#ifndef SYNERGYSPOT_CVPROVIDEOSTRATEGY_H
#define SYNERGYSPOT_CVPROVIDEOSTRATEGY_H

#include "../EffectComponentPlugin.h"
#include <QImage>
#include <opencv2/opencv.hpp>

#include "help.h"

class CVProVideoStrategy : public BackgroundStrategyPlugin{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.synergy-spot.SS_PluginInterface")
    Q_INTERFACES(SS_Plugin)
public:
    CVProVideoStrategy() : cap(nullptr){}
    ~CVProVideoStrategy() override
    {
        if(cap){
            cap->release();
            delete cap;
        }
    }

    // initial MP4 video / camera source
    bool initialize(const QString& videoSrc) override;

    // test camera is available
    bool testCamera();

    // release camera source
    void releaseCamera();

    // update video frame and make video get in loop
    void updateFrame() override;

    // get cur frame in the video
    QImage getCurrentFrame() const override;

    // is the video finished
    bool isFinished() const override;

    // reset the video
    void reset() override;

    virtual const char* getPluginName() override{
        return BK_PLUGIN_NAME;
    }
private:
    cv::VideoCapture* cap;
    cv::Mat frame;
    QImage currentFrame;
    bool isCamera;
};


#endif//SYNERGYSPOT_CVPROVIDEOSTRATEGY_H
