//
// Created by developer on 8/31/17.
//

/**
 * 拉取rtmp流,保存本地文件
 */

#include <memory>
#include <thread>
#include <iostream>

int64_t lastReadPacktTime;
int stopPull;
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/opt.h"
#include "include/libavutil/frame.h"
#include "include/libavutil/time.h"
#include "include/libavutil/opt.h"
#include "include/libavutil/channel_layout.h"
#include "include/libavutil/common.h"
#include "include/libavutil/imgutils.h"
#include "include/libavutil/fifo.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avio.h"
#include "include/libavfilter/avfiltergraph.h"
#include "include/libavfilter/avfilter.h"
#include "include/libavfilter/buffersink.h"
#include "include/libavfilter/buffersrc.h"
#include "include/libswscale/swscale.h"
#include "libswresample/swresample.h"


#include <jni.h>
#include <android/log.h>
#include "debug.h"

AVFormatContext *av_input_fmt = NULL;
AVFormatContext *av_output_fmt = NULL;

void custom_pullstream_log(void *ptr, int level, const char *fmt, va_list vaList) {

    FILE *fHandler = fopen("/storage/emulated/0/av_pull_log.txt", "a+");
    if (fHandler) {
        vfprintf(fHandler, fmt, vaList);
        fflush(fHandler);
        fclose(fHandler);
    }
}

shared_ptr<AVPacket> ReadPacketFromSource() {
    shared_ptr<AVPacket> packet(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                [&](AVPacket *p) {
                                    av_packet_free(&p);
                                    av_freep(&p);
                                });
    av_init_packet(packet.get());
    lastReadPacktTime = av_gettime();
    int ret = av_read_frame(av_input_fmt, packet.get());
    if (ret >= 0) {
        return packet;
    }
    else {
        return NULL;
    }
}

void av_packet_rescale_ts(AVPacket *pkt, AVRational src_tb, AVRational dst_tb) {
    if (pkt->pts != AV_NOPTS_VALUE)
        pkt->pts = av_rescale_q(pkt->pts, src_tb, dst_tb);
    if (pkt->dts != AV_NOPTS_VALUE)
        pkt->dts = av_rescale_q(pkt->dts, src_tb, dst_tb);
    if (pkt->duration > 0)
        pkt->duration = av_rescale_q(pkt->duration, src_tb, dst_tb);
}
int WritePacket(shared_ptr<AVPacket> packet) {
    auto inputStream = av_input_fmt->streams[packet->stream_index];
    auto outputStream = av_output_fmt->streams[packet->stream_index];
    av_packet_rescale_ts(packet.get(), inputStream->time_base, outputStream->time_base);
    return av_interleaved_write_frame(av_output_fmt, packet.get());
}

void CloseInput() {
    if (av_input_fmt != nullptr) {
        avformat_close_input(&av_output_fmt);
    }
}

void CloseOutput() {
    if (av_output_fmt != nullptr) {
        for (int i = 0; i < av_output_fmt->nb_streams; i++) {
            AVCodecContext *codecContext = av_output_fmt->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&av_output_fmt);
    }
}
void Init() {
    av_log_set_callback(custom_pullstream_log);
    av_log_set_level(AV_LOG_ERROR);
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
    av_input_fmt = avformat_alloc_context();
}

int openOutput(const char *outputUri) {

    int ret;

    if ((ret = avformat_alloc_output_context2(&av_output_fmt, NULL, "flv", outputUri)) < 0) {
        LOG_E(DEBUG, "avformat_alloc_output_context2 error")
        goto Error;
    }
    ret = avio_open2(&av_output_fmt->pb, outputUri, AVIO_FLAG_READ_WRITE, NULL, NULL);
    if (ret < 0) {
        LOG_E(DEBUG, "avio_open2");
        goto Error;
    }

    for (int i = 0; i < av_input_fmt->nb_streams; ++i) {
        AVStream *avStream = avformat_new_stream(av_output_fmt,
                                                 av_input_fmt->streams[i]->codec->codec);
        ret = avcodec_copy_context(avStream->codec, av_input_fmt->streams[i]->codec);

        if (ret < 0) {
            LOG_E(DEBUG, "avcodec_copy_context error!");
            goto Error;
        }
    }

    ret = avformat_write_header(av_output_fmt, NULL);


    if (ret < 0) {
        LOG_E(DEBUG, "avformat write header!");
        goto Error;
    }
    return ret;

    Error:
    if (av_output_fmt) {
        for (int i = 0; i < av_input_fmt->nb_streams; ++i) {
            avcodec_close(av_output_fmt->streams[i]->codec);
        }
        avformat_close_input(&av_output_fmt);
    }

    return ret;
}


int openInput(const char *inputUri) {

//    av_log_set_callback(custom_pullstream_log);
//    av_register_all();
//    avformat_network_init();
//    av_input_fmt = avformat_alloc_context();

    int ret;
    if ((ret = avformat_open_input(&av_input_fmt, inputUri, NULL, NULL)) < 0) {

        LOG_E(DEBUG, "avformat open input");
        return -1;
    }

    if ((ret = avformat_find_stream_info(av_input_fmt, NULL)) < 0) {

        LOG_E(DEBUG, "avformat find stream info");
        return -1;
    }

    return ret;

}


JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_pullStream(JNIEnv *env, jclass type,
                                                                      jstring inputUri_,
                                                                      jstring outputUri_) {
    const char *inputUri = env->GetStringUTFChars(inputUri_, 0);
    const char *outputUri = env->GetStringUTFChars(outputUri_, 0);


    FILE *fHandle = fopen(outputUri, "wb+");
    if (!fHandle) {
        LOG_E(DEBUG, "fopen error!");
        return -1;
    }
    Init();
    int ret = openInput(inputUri);
    if (ret >= 0) {
        ret = openOutput(outputUri);
    }
    if (ret < 0) goto Error;

    while (true) {
        if (stopPull) {
            stopPull=0;
            break;
        }
        auto packet = ReadPacketFromSource();
        if (packet) {
            ret = WritePacket(packet);
            if (ret >= 0) {
                LOG_D(DEBUG, "WritePacket Success!");
            } else {
                LOG_D(DEBUG, "WritePacket failed!");
            }
        }
        else {
            break;
        }
    }

    Error:
    CloseInput();
    CloseOutput();
//    while (true) {
//        this_thread::sleep_for(chrono::seconds(100));
//    }
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_stopPull(JNIEnv *env, jclass type) {


    stopPull = 1;
    return 0;

}

#ifdef __cplusplus
}
#endif