//
// Created by developer on 11/12/17.
//

#include "AudioCapture.h"

AudioCapture::AudioCapture() {

}

AudioCapture::~AudioCapture() {
    if (NULL != audioEncodeArgs) {
        delete audioEncodeArgs;
    }
}

AudioCapture *AudioCapture::Get() {
    static AudioCapture audioCapture;
    return &audioCapture;
}

/**
 * 资源回收
 */
int AudioCapture::Release() {
    LOG_D(DEBUG, "Release Audio Capture!");
    return 0;
}

/**
 * 
 */
bool AudioCapture::CloseCapture() {
    std::lock_guard<std::mutex> lk(mut);
    if (!ExitCapture) {
        ExitCapture = true;
        LOG_D(DEBUG, "Close Audio Capture");
    }
    return ExitCapture;
}


bool AudioCapture::StartCapture() {
    std::lock_guard<std::mutex> lk(mut);
    ExitCapture = false;
    LOG_D(DEBUG, "Start Audio Capture");
    return !ExitCapture;
}


void AudioCapture::SetAudioEncodeArgs(AudioEncodeArgs *audioEncodeArgs) {
    this->audioEncodeArgs = audioEncodeArgs;
}

AudioEncodeArgs *AudioCapture::GetAudioEncodeArgs() {
    return this->audioEncodeArgs;
}

bool AudioCapture::GetCaptureState() {
    return ExitCapture;
}

/**
 * 往除列添加音频原数据
 */
int AudioCapture::PushAudioData(OriginData *originData) {
    if (ExitCapture) {
        return 0;
    }
    originData->pts = av_gettime();
    LOG_D(DEBUG,"audio capture pts :%lld",originData->pts);
    audioCaputureframeQueue.push(originData);
    return 0;
}

OriginData *AudioCapture::GetAudioData() {
    if (ExitCapture) {
        return NULL;
    }
    if (audioCaputureframeQueue.empty()) {
        return NULL;
    } else {
        const shared_ptr<OriginData *> &ptr = audioCaputureframeQueue.try_pop();
        return NULL == ptr ? NULL : *ptr.get();
    }
}

