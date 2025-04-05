#pragma once

#ifndef TRTCCLOUDCALLBACKDEFAULTIMPL_H
#define TRTCCLOUDCALLBACKDEFAULTIMPL_H

#include "TRTC/ITRTCCloud.h"
#include "TRTC/TRTCCloudCallback.h"
#include "TRTC/TRTCTypeDef.h"
#include "GenerateUserSig.h"
#include "TRTC/ITXDeviceManager.h"

class TRTCCloudCallbackDefaultImpl : public liteav::ITRTCCloudCallback
{
public:
    virtual void onWarning(TXLiteAVWarning warningCode, const char* warningMsg, void* extraInfo) override{

    };
    virtual void onError(TXLiteAVError errCode, const char *errMsg, void *extraInfo) override{

    }

    virtual void onEnterRoom(int result) override{

    };
    virtual void onExitRoom(int reason) override{

    };
};

#endif // TRTCCLOUDCALLBACKDEFAULTIMPL_H
