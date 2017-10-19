//
// Created by developer on 9/7/17.
//网络流转发实例类
//

#ifndef NATIVEAPP_JNI_TRANS_STREAM_H
#define NATIVEAPP_JNI_TRANS_STREAM_H

#include "base_include.h"
#include <memory>
#include <thread>
#include <iostream>

using namespace std;


typedef void (*custom_callack)(void *ptr, int level, const char *fmt, va_list valist);

class TransStream {

public:

    AVFormatContext *avInputFmCxt=NULL;
    AVFormatContext *avOuputFmCxt=NULL;

    bool isStop;

    TransStream();

    virtual ~TransStream();

    /**
     * 初始化
     */
    int Init();

    shared_ptr<AVPacket> ReadPacketFromSource();

    int WritePacket(shared_ptr<AVPacket> packet);

    void av_packet_rescale_ts(AVPacket *pkt, AVRational src_tb, AVRational dst_tb);

    int openInput(const char *inputUri);

    int openOuput(const char *outputUri);

    int trans_stream(const char *inputUri, const char *outputUri);

    int stopStream();

    int64_t lastReadPacktTime;

    custom_callack custom_log_callack;
};


#endif