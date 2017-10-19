//
// Created by developer on 9/7/17.
//

#include "jni_trans_stream.h"


TransStream::TransStream() {
    this->avInputFmCxt = NULL;
    this->avOuputFmCxt = NULL;
}

TransStream::~TransStream() {
}


shared_ptr<AVPacket> TransStream::ReadPacketFromSource() {
    shared_ptr<AVPacket> packet(static_cast<AVPacket *>(av_malloc(sizeof(AVPacket))),
                                [&](AVPacket *p) {
                                    av_packet_free(&p);
                                    av_freep(&p);
                                });
    int ret = 0;
    av_init_packet(packet.get());
    lastReadPacktTime = av_gettime();
    ret = av_read_frame(avInputFmCxt, packet.get());
    if (ret >= 0) {
        return packet;
    } else {
        return NULL;
    }
}

void TransStream::av_packet_rescale_ts(AVPacket *pkt, AVRational src_tb, AVRational dst_tb) {
    if (pkt->pts != AV_NOPTS_VALUE)
        pkt->pts = av_rescale_q(pkt->pts, src_tb, dst_tb);
    if (pkt->dts != AV_NOPTS_VALUE)
        pkt->dts = av_rescale_q(pkt->dts, src_tb, dst_tb);
    if (pkt->duration > 0)
        pkt->duration = av_rescale_q(pkt->duration, src_tb, dst_tb);
}

int TransStream::WritePacket(shared_ptr<AVPacket> packet) {

    auto inputStream = avInputFmCxt->streams[packet->stream_index];
    auto outputStream = avOuputFmCxt->streams[packet->stream_index];
    av_packet_rescale_ts(packet.get(), inputStream->time_base, outputStream->time_base);
    return av_interleaved_write_frame(avOuputFmCxt, packet.get());
}

void log_callack(void *ptr, int level, const char *fmt, va_list valist) {
    FILE *fHandle = fopen("/storage/emulated/0/av_trans_log.txt", "a+");
    if (fHandle) {
        vfprintf(fHandle, fmt, valist);
        fflush(fHandle);
        fclose(fHandle);
    } else {
        LOG_E(DEBUG, "file open failed!");
    }
}


int TransStream::openInput(const char *inputUri) {

    avInputFmCxt = avformat_alloc_context();
    int ret = 0;
    if ((ret = avformat_open_input(&avInputFmCxt, inputUri, NULL, NULL)) < 0) {
        return ret;
    }

    if ((ret = avformat_find_stream_info(avInputFmCxt, NULL)) < 0) {
        return ret;
    }
    return ret;
}

int TransStream::openOuput(const char *outputUri) {
    int ret;


//    FILE *fHandle = fopen(outputUri, "wb+");
//    if (!fHandle) {
//        LOG_E(DEBUG, "fopen error!");
//        return -1;
//    }
    char *buf = static_cast<char *>(malloc(2000));
    if ((ret = avformat_alloc_output_context2(&avOuputFmCxt, NULL, "flv", outputUri)) < 0) {
        LOG_E(DEBUG, "avformat_alloc_output_context2 error")
        goto Error;
    }
    ret = avio_open2(&avOuputFmCxt->pb, outputUri, AVIO_FLAG_READ_WRITE, NULL, NULL);
    if (ret < 0) {
        LOG_E(DEBUG, "avio_open2");
        goto Error;
    }


    for (int i = 0; i < avInputFmCxt->nb_streams; ++i) {
        AVStream *avStream = avformat_new_stream(avOuputFmCxt,
                                                 avInputFmCxt->streams[i]->codec->codec);
        ret = avcodec_copy_context(avStream->codec, avInputFmCxt->streams[i]->codec);

        if (ret < 0) {
            LOG_E(DEBUG, "avcodec_copy_context error!");
            goto Error;
        }
    }


    ret = avformat_write_header(avOuputFmCxt, NULL);
    av_strerror(ret, buf, sizeof(buf));
    printf("%s", buf);
    delete[] buf;

    if (ret < 0) {
        LOG_E(DEBUG, "avformat write header!");
        goto Error;
    }
    return ret;


    Error:
    if (avOuputFmCxt) {
        for (int i = 0; i < avOuputFmCxt->nb_streams; ++i) {
            avcodec_close(avOuputFmCxt->streams[i]->codec);
        }
        avformat_close_input(&avOuputFmCxt);
    }

    return ret;
}

int TransStream::Init() {
    custom_log_callack = log_callack;
    av_log_set_callback(custom_log_callack);
    av_log_set_level(AV_LOG_DEBUG);
    av_register_all();
    avfilter_register_all();
    avformat_network_init();
    return 0;
}


int TransStream::trans_stream(const char *inputUri, const char *outputUri) {


    return 0;
}


int TransStream::stopStream() {
    if (avInputFmCxt != nullptr) {
        avformat_close_input(&avInputFmCxt);
    }
    if (avOuputFmCxt != nullptr) {
        for (int i = 0; i < avOuputFmCxt->nb_streams; i++) {
            AVCodecContext *codecContext = avOuputFmCxt->streams[i]->codec;
            avcodec_close(codecContext);
        }
        avformat_close_input(&avOuputFmCxt);
    }
    return 0;
}