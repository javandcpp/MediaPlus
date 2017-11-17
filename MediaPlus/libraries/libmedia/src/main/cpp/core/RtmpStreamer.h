//
// Created by developer on 11/14/17.
//

#include "MediaStreamer.h"

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

#ifndef NATIVEAPP_RTMPSTREAMER_H
#define NATIVEAPP_RTMPSTREAMER_H

using namespace std;

/**
 * Rtmp推流器
 */
class RtmpStreamer : public MediaStreamer {

private:
    RtmpStreamer();


public:
    ~RtmpStreamer();

    bool writeHeadFinish=false;

    pthread_t t1;
    pthread_t t2;
    pthread_t t3;

    static RtmpStreamer *Get();

    static void *PushAudioStreamTask(void *pObj);

    static void *PushVideoStreamTask(void *pObj);

    static void *PushStreamTask(void *pObj);

    static void *WriteHead(void *pObj);

    int InitStreamer(const char *url);

    /**
     * 设置音频编码器
     */
    int SetAudioEncoder(AudioEncoder *audioEncoder);

    /**
     * 设置视频编码器
     */
    int SetVideoEncoder(VideoEncoder *videoEncoder);

    /**
     * 添加音视频流
     */
    int AddStream(AVCodecContext *avCodecContext);

    /**
     * 写入头
     */


    /**
     * 开启推流
     */
    int StartPushStream();

    /**
    * 关闭推流
    */
    int ClosePushStream();

    int SendAudioFrame(OriginData *originData, int streamIndex);

    int SendVideoFrame(OriginData *originData, int streamIndex);

    int SendFrame(OriginData *pData, int i);
};

#endif //NATIVEAPP_RTMPSTREAMER_H
