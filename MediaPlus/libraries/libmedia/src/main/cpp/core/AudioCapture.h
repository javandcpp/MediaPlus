//
// Created by developer on 11/12/17.
//


#ifndef NATIVEAPP_AUDIOCAPTURE_H
#define NATIVEAPP_AUDIOCAPTURE_H

#include <list>
#include "MediaCapture.h"
#include "../PrefixHeader.h"

using namespace std;

/**
 * 音频数据接收
 */
class AudioCapture : public MediaCapture {

private:
    AudioCapture();

    AudioEncodeArgs *audioEncodeArgs = NULL;
public :
    ~AudioCapture();

//    list<OriginData *> AudioCaptureDatalist;

    threadsafe_queue<OriginData *> audioCaputureframeQueue;

    static AudioCapture *Get();

    bool CloseCapture();

    bool StartCapture();

    int Release();

    int PushAudioData(OriginData *originData);

    OriginData *GetAudioData();
    /**
   * 设置编码参数
   */
    void SetAudioEncodeArgs(AudioEncodeArgs *audioEncodeArgs);

    AudioEncodeArgs *GetAudioEncodeArgs();

    /**
     * 获取音频采集状态
     */
    bool GetCaptureState();



};

#endif //NATIVEAPP_AUDIOCAPTURE_H
