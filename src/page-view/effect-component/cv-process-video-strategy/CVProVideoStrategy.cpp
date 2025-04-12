//
// Created by FU-QAQ on 2024/12/7.
//

#include "CVProVideoStrategy.h"
#include "help.h"

#include <QFileInfo>

extern "C" SS_API BackgroundStrategyPlugin* createCVProVideoStrategyPlugin(){
    return new CVProVideoStrategy();
}

bool CVProVideoStrategy::initialize(const QString &videoSrc) {
    const int MAX_CAMERA_INDEX = 10;

    // 优先处理存在的文件
    QFileInfo fileInfo(videoSrc);
    if (fileInfo.exists()) {
        cap = new cv::VideoCapture(videoSrc.toStdString());
        if (cap->isOpened()) {
            isCamera = false;
            return true;
        }
        LOG("文件存在但打开失败: " + videoSrc.toStdString());
        delete cap;
        cap = nullptr;
    }

    // 尝试作为摄像头索引处理
    bool ok;
    int userCameraIndex = videoSrc.toInt(&ok);
    if (ok) {
        cap = new cv::VideoCapture(userCameraIndex);
        if (cap->isOpened() && testCamera()) {
            isCamera = true;
            return true;
        }
        delete cap;
    }

    // 轮询其他摄像头
    for (int i = 0; i < MAX_CAMERA_INDEX; ++i) {
        if (ok && i == userCameraIndex) continue;
        cap = new cv::VideoCapture(i);
        if (cap->isOpened() && testCamera()) {
            isCamera = true;
            return true;
        }
        delete cap;
    }

    // 尝试作为视频文件（支持网络流/虚拟文件）
    cap = new cv::VideoCapture(videoSrc.toStdString());
    if (cap->isOpened()) {
        isCamera = false;
        return true;
    }

    LOG_ERROR("所有视频源打开失败");
    return false;
}

bool CVProVideoStrategy::testCamera() {
    if (!cap->isOpened()) return false;
    cv::Mat testFrame;
    for (int i = 0; i < 5; ++i) { // 尝试5次读取
        if (cap->read(testFrame) && !testFrame.empty()) {
            return true;
        }
    }
    return false;
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
    if (!cap || !cap->isOpened()) {
        if (isCamera) {
            // 尝试重新初始化摄像头
            releaseCamera();
            initialize("0");
        }
        return;
    }

    if (cap->read(frame)) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
        currentFrame = QImage(frame.data, frame.cols, frame.rows,
                            frame.step, QImage::Format_RGB888).copy();
    } else if (!isCamera) {
        cap->set(cv::CAP_PROP_POS_FRAMES, 0); // 循环视频文件
    } else {
        // 摄像头断开处理
        releaseCamera();
        initialize("0");
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
