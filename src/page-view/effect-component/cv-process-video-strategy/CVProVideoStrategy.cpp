//
// Created by FU-QAQ on 2024/12/7.
//

#include "CVProVideoStrategy.h"
#include "help.h"

extern "C" SS_API BackgroundStrategyPlugin* createCVProVideoStrategyPlugin(){
    return new CVProVideoStrategy();
}

bool CVProVideoStrategy::initialize(const QString &videoSrc) {
    bool ok;
    int cameraIndex = videoSrc.toInt(&ok);       // 通过字符串判断启动是摄像头还是视频文件
    if (ok) {
        cap = new cv::VideoCapture(cameraIndex); // 摄像头初始化
        isCamera = true;
    } else {
        cap = new cv::VideoCapture(videoSrc.toStdString()); // 文件初始化
        isCamera = false;
    }
    if (!cap->isOpened()) {
        LOG("Failed to open video source");
        return false;
    }
    return true;
}

void CVProVideoStrategy::releaseCamera() {
    if (cap && isCamera) {
        cap->release();
        delete cap;
        cap = nullptr;
        isCamera = false;
    }
    currentFrame = QImage();
}

void CVProVideoStrategy::updateFrame() {
    if (cap && cap->read(frame)) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        currentFrame = QImage(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888).copy();
    } else {
        cap->set(cv::CAP_PROP_POS_FRAMES, 0); // Loop video
    }
}

QImage CVProVideoStrategy::getCurrentFrame() const {
    return currentFrame;
}

bool CVProVideoStrategy::isFinished() const {
    return isCamera ? false : (cap->get(cv::CAP_PROP_POS_FRAMES) >= cap->get(cv::CAP_PROP_FRAME_COUNT));
}

void CVProVideoStrategy::reset() {
    if(cap){
        cap->set(cv::CAP_PROP_POS_FRAMES,0);
    }
}
