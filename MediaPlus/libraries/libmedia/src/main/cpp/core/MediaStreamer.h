//
// Created by developer on 11/14/17.
//

#ifndef NATIVEAPP_MEDIARTMPSTREAMER_H
#define NATIVEAPP_MEDIARTMPSTREAMER_H

#include "MediaEncoder.h"
#include "AudioEncoder.h"
#include "../PrefixHeader.h"
#include "VideoEncoder.h"
#include "debug.h"

class MediaStreamer {

protected:
    MediaStreamer();


    mutable std::mutex mut;
    mutable std::mutex mut1;
    mutable std::mutex mut2;

    AudioEncoder *audioEncoder = NULL;
    VideoEncoder *videoEncoder = NULL;

    AVStream *audioStream = NULL;
    AVStream *videoStream = NULL;

    AVCodecContext *audioCodecContext = NULL;
    AVCodecContext *videoCodecContext = NULL;
    AVFormatContext *iAvFormatContext = NULL;


    int videoStreamIndex = -1;
    int audioStreamIndex = -1;

    const char *outputUrl = NULL;

    bool isPushStream=false;

    ~MediaStreamer();

    /**
     * 初始化推流器
     */
    virtual int InitStreamer(const char *url) = 0;

    /**
     * 设置推流音频编码器,用于获取已编码数据
     */
    virtual int SetAudioEncoder(AudioEncoder *audioEncoder) = 0;

    /**
     * 设置推流视频编码器,用于获取已编码数据
     */
    virtual int SetVideoEncoder(VideoEncoder *videoEncoder) = 0;

    /**
     * 添加音视频流
     */
    virtual int AddStream(AVCodecContext *avCodecContext) = 0;

    /**
     * 开始推流
     */
    virtual int StartPushStream() = 0;

    /**
     * 关闭推流
     */
    virtual int ClosePushStream() = 0;
};

#endif //NATIVEAPP_MEDIARTMPSTREAMER_H
