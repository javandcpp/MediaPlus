//
// Created by developer on 8/29/17.
//


#ifdef __cplusplus
extern "C"{
#endif
int isStop;


#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/opt.h"
#include "include/libavutil/frame.h"
#include "include/libavutil/time.h"
#include <jni.h>
#include <android/log.h>
#include "debug.h"


void custom_log(void *ptr, int level, const char *fmt, va_list vl) {

    //To TXT file
    FILE *fp = fopen("/storage/emulated/0/av_log.txt", "a+");
    if (fp) {
        vfprintf(fp, fmt, vl);
        fflush(fp);
        fclose(fp);
    }

    //To Logcat
    //LOGE(fmt, vl);
}


JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_stopPush(JNIEnv *env, jclass type) {
    isStop = 1;
    return 0;
}


int endPush(AVFormatContext *ifmt_ctx, int ret, AVOutputFormat *ofmt, AVFormatContext *ofmt_ctx) {
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_close(ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    if (ret < 0 && ret != AVERROR_EOF) {
        LOG_E(DEBUG, "Error occurred.\n");
        return -1;
    }
    return 0;
}


JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_pushStream(JNIEnv *env, jclass type,
                                                                        jstring inputUri_,
                                                                        jstring outputUri_) {

    const char *inputUri = (*env)->GetStringUTFChars(env,inputUri_, 0);
    const char *outputUri = (*env)->GetStringUTFChars(env,outputUri_, 0);
    AVOutputFormat *ofmt=NULL;
    AVFormatContext *ifmt_ctx=NULL;
    AVFormatContext *ofmt_ctx=NULL;
    AVPacket pkt;

    int ret, i;
    isStop=0;

    //FFmpeg av_log() callback
    av_log_set_callback(custom_log);

    av_register_all();
    //Network
    avformat_network_init();

    ret = avformat_open_input(&ifmt_ctx, inputUri, NULL, NULL);
    if (ret < 0) {
        LOG_E(DEBUG, "Could not open input file.");
        return -1;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, NULL)) < 0) {
        LOG_E(DEBUG, "Failed to retrieve input stream information");
        return -1;
    }

    int videoindex = -1;
    for (i = 0; i < ifmt_ctx->nb_streams; i++)
        if (ifmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    //Output

   // AVOutputFormat *outputFormat=av_guess_format("flv",NULL,NULL);//用于测试flv复用器是否打开
   // outputFormat=av_guess_format(NULL,outputUri,NULL);//用于测试flv复用器是否打开


//    --enable-muxer=flv

// 编译配置,添加此选项
// ,avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", outputUri)
// 才能执行成功
    avformat_alloc_output_context2(&ofmt_ctx, NULL, "flv", outputUri); //RTMP
//    avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", outputUri);//UDP

    if (!ofmt_ctx) {
        LOG_E(DEBUG, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return -1;
    }
    ofmt = ofmt_ctx->oformat;
    for (i = 0; i < ifmt_ctx->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        AVStream *in_stream = ifmt_ctx->streams[i];
        AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
        if (!out_stream) {
            LOG_E(DEBUG, "Failed allocating output stream\n");
            ret = AVERROR_UNKNOWN;
            return -1;
        }
        //Copy the settings of AVCodecContext
        ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
        if (ret < 0) {
            LOG_E(DEBUG, "Failed to copy context from input to output stream codec context\n");
            return -1;
        }
        out_stream->codec->codec_tag = 0;
        if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
            out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    //Open output URL
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, outputUri, AVIO_FLAG_WRITE);//avio_open 实际调用avio_open2
        if (ret < 0) {
            LOG_E(DEBUG, "Could not open output URL '%s'", outputUri);
            return -1;
        }
    }
    //Write file header
    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        LOG_E(DEBUG, "Error occurred when opening output URL\n");
        return -1;
    }

    int frame_index = 0;

    int64_t start_time = av_gettime();
    while (1) {
        if (isStop) {
            break;
        }
        AVStream *in_stream, *out_stream;
        //Get an AVPacket
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        //FIX：No PTS (Example: Raw H.264)
        //Simple Write PTS
        if (pkt.pts == AV_NOPTS_VALUE) {
            //Write PTS
            AVRational time_base1 = ifmt_ctx->streams[videoindex]->time_base;
            //Duration between 2 frames (us)
            int64_t calc_duration =
                    (double) AV_TIME_BASE / av_q2d(ifmt_ctx->streams[videoindex]->r_frame_rate);
            //Parameters
            pkt.pts = (double) (frame_index * calc_duration) /
                      (double) (av_q2d(time_base1) * AV_TIME_BASE);
            pkt.dts = pkt.pts;
            pkt.duration = (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
        }
        //Important:Delay
        if (pkt.stream_index == videoindex) {
            AVRational time_base = ifmt_ctx->streams[videoindex]->time_base;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);

        }

        in_stream = ifmt_ctx->streams[pkt.stream_index];
        out_stream = ofmt_ctx->streams[pkt.stream_index];
        /* copy packet */
        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base,
                                   AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        //Print to Screen
        if (pkt.stream_index == videoindex) {
            LOG_E(DEBUG, "Send %8d video frames to output URL\n", frame_index);
            frame_index++;
        }
        //ret = av_write_frame(ofmt_ctx, &pkt);
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);

        if (ret < 0) {
            LOG_E(DEBUG, "Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(ofmt_ctx);

    return endPush(ifmt_ctx, ret, ofmt, ofmt_ctx);

}

#ifdef __cplusplus
}
#endif
