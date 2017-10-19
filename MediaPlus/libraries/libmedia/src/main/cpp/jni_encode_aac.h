//
// Created by developer on 8/18/17.
//

#ifndef NATIVEAPP_JNI_ENCODE_AAC_H
#define NATIVEAPP_JNI_ENCODE_AAC_H


#include "base_include.h"
#include "include/libavutil/frame.h"


class AACEncode {


public:
    AACEncode(UserArguments *arguments);

    int initAudioEncoder();

    static void *startEncode(void *obj);

    void user_end();

    void release();

    int sendOneFrame(uint8_t *buf);

    int encodeEnd();

    ~AACEncode();

private:
    int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index);
    threadsafe_queue<uint8_t *> frame_queue;
    AVFormatContext *pFormatCtx;
    AVOutputFormat *fmt;
    AVStream *audio_st;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;

    AVFrame *pFrame;
    AVPacket pkt;

    int got_frame = 0;
    int ret = 0;
    int size = 0;

    int i;
    int is_end=0;
    int is_release=0;
    UserArguments *userArguments;

};


#endif //NATIVEAPP_JNI_ENCODE_AAC_H
