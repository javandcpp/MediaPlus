//
// Created by developer on 8/17/17.
//



#ifndef NATIVEAPP_H264ENCODE_H
#define NATIVEAPP_H264ENCODE_H

#include <cstdint>
#include "base_include.h"

#ifdef  _cplusplus
extern "C"{
#endif


class H264Encode {

public:
    H264Encode(UserArguments *userArguments);

    virtual ~H264Encode();

    int initVideoEncoder();

    static void *startEncode(void *obj);

    void user_end();

    void release();

    int sendOneFrame(uint8_t *buf);

    int encodeEnd();

    void custom_filter(const H264Encode *h264_encoder, const uint8_t *picture_buf,
                       int in_y_size,
                       int format);

private:

    int is_end = 0;
    int is_release = 0;
    threadsafe_queue<uint8_t *> frame_queue;
    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *video_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket pkt;
    AVFrame *pFrame;
    int picture_size;
    int out_y_size;
    int framecnt = 0;
    int frame_count = 0;
    UserArguments *arguments;

};

#ifdef _cplusplus
}
#endif

#endif //NATIVEAPP_H264ENCODE_H
