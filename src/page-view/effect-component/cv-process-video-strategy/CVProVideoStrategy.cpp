//
// Created by FU-QAQ on 2024/12/7.
//

#include "CVProVideoStrategy.h"
#include "help.h"

extern "C" SS_API BackgroundStrategyPlugin* createCVProVideoStrategyPlugin(){
    return new CVProVideoStrategy();
}

bool CVProVideoStrategy::initialize(const QString &videoSrc) {
    cap = new cv::VideoCapture(videoSrc.toStdString());
    if (!cap->isOpened()) {
        LOG("Failed to open video source");
        return false;
    }
    return true;
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
    return cap->get(cv::CAP_PROP_POS_FRAMES) >= cap->get(cv::CAP_PROP_FRAME_COUNT);
}

void CVProVideoStrategy::reset() {
    if(cap){
        cap->set(cv::CAP_PROP_POS_FRAMES,0);
    }
}
