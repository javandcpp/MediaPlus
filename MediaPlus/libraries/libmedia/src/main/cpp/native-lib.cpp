#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <jni.h>

#include <string.h>
#include <stdio.h>
#include "debug.h"



#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#include "sdl2/include/SDL.h"
#include "sdl2/include/SDL_log.h"
#include "sdl2/include/SDL_main.h"



JNIEXPORT jstring JNICALL
Java_com_guagua_nativeapp_MainActivity_nativeString(JNIEnv *evn, jobject obj) {
    avcodec_register_all();
    const char *string = avcodec_configuration();
    char *info = new char[strlen(string)];
    sprintf(info, "%s\n", string);
    return evn->NewStringUTF(info);
}

JNIEXPORT jstring JNICALL
Java_com_guagua_nativeapp_MainActivity_nativeGetVideoInfo(JNIEnv *env, jobject instance,
                                                          jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVCodec *pEncoder;
    AVFrame *pFrame, *pFrameYUV;
    unsigned char *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    int screen_w = 0, screen_h = 0;
    FILE *fp_yuv;

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();


    if (avformat_open_input(&pFormatCtx, path, NULL, NULL) != 0) {

        LOG_D("native", "-------------open file failed!-----------");
        return env->NewStringUTF("Couldn't open input stream");
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOG_D("native", "can't find stream info");
        return env->NewStringUTF("Couldn't find stream information.");
    }
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    if (videoindex == -1) {
        LOG_D(DEBUG,"Didn't find a video stream.\n");
        return env->NewStringUTF("Didn't find a video stream");
    }

    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pEncoder = avcodec_find_encoder(pCodecCtx->codec_id);
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOG_D(DEBUG,"Codec not found.\n");
        return env->NewStringUTF("Codec not found");
    } else {
        LOG_D(DEBUG,"encoder :%s", pEncoder->name);
    }

    if (pEncoder != NULL) {
        LOG_D(DEBUG,"decoder:%s", pEncoder->name);

    }

    int64_t duration = pFormatCtx->duration / 1000 / 1000;

    LOG_D(DEBUG,"duration时长:%lld(秒)", duration);

    LOG_D(DEBUG,"文件名:", "%s", pFormatCtx->filename);
    LOG_D(DEBUG,"bitrate码率:%lld", pFormatCtx->bit_rate);


    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        LOG_D("native", "could not open codec");
        return env->NewStringUTF("Could not open codec");
    }


    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    //Output Info-----------------------------
    LOG_D("native", "--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx, 0, path, 0);
    LOG_D("native", "-------------------------------------------------\n");
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);


    avformat_close_input(&pFormatCtx);
    env->ReleaseStringUTFChars(path_, path);
    return env->NewStringUTF(avcodec_configuration());
}

JNIEXPORT jstring JNICALL
Java_com_guagua_nativeapp_MainActivity_getStringUTF(JNIEnv *env, jobject instance) {

    return env->NewStringUTF("1111122222");


}


void callback(void *v, int i, const char *a, va_list b) {
    LOG_D(DEBUG,"%d --%s", i, a);
}

JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_MainActivity_nativeDecode(JNIEnv *env, jobject instance,
                                                    jstring inputUri_, jstring outputUri_) {
    const char *inputUri = env->GetStringUTFChars(inputUri_, 0);
    const char *outputUri = env->GetStringUTFChars(outputUri_, 0);
    AVFormatContext *pFormatCtx;
    int i, videoindex;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int y_size;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    FILE *fp_yuv;
    int frame_cnt;
    clock_t time_start, time_finish;
    double time_duration = 0.0;

//    char input_str[500]={0};
//    char output_str[500]={0};
    char info[1000] = {0};
    const char *input_str = inputUri;
    const char *output_str = outputUri;
//    sprintf(output_str,"%s",(*env)->GetStringUTFChars(env,outputUri_, NULL));
//
    //FFmpeg av_log() callback

    av_log_set_callback(&callback);

    av_register_all();
    avformat_network_init();
    pFormatCtx = avformat_alloc_context();

    if (avformat_open_input(&pFormatCtx, input_str, NULL, NULL) != 0) {
        LOG_E(DEBUG,"Couldn't open input stream.\n");
        return -1;
    }
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOG_E(DEBUG,"Couldn't find stream information.\n");
        return -1;
    }
    videoindex = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++)
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoindex = i;
            break;
        }
    if (videoindex == -1) {
        LOG_E(DEBUG,"Couldn't find a video stream.\n");
        return -1;
    }
    pCodecCtx = pFormatCtx->streams[videoindex]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOG_E(DEBUG,"Couldn't find Codec.\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOG_E(DEBUG,"Couldn't open codec.\n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (unsigned char *) av_malloc(
            av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);


    packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
                                     pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                     SWS_BICUBIC, NULL, NULL, NULL);


    sprintf(info, "[Input     ]%s\n", input_str);
    sprintf(info, "%s[Output    ]%s\n", info, output_str);
    sprintf(info, "%s[Format    ]%s\n", info, pFormatCtx->iformat->name);
    sprintf(info, "%s[Codec     ]%s\n", info, pCodecCtx->codec->name);
    sprintf(info, "%s[Resolution]%dx%d\n", info, pCodecCtx->width, pCodecCtx->height);


    fp_yuv = fopen(output_str, "wb+");
    if (fp_yuv == NULL) {
        printf("Cannot open output file.\n");
        return -1;
    }

    frame_cnt = 0;
    time_start = clock();

    while (av_read_frame(pFormatCtx, packet) >= 0) {
        if (packet->stream_index == videoindex) {
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if (ret < 0) {
                LOG_E(DEBUG,"Decode Error.\n");
                return -1;
            }
            if (got_picture) {
                sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize,
                          0, pCodecCtx->height,
                          pFrameYUV->data, pFrameYUV->linesize);

                y_size = pCodecCtx->width * pCodecCtx->height;
                fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
                fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
                fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
                //Output info
                char pictype_str[10] = {0};
                switch (pFrame->pict_type) {
                    case AV_PICTURE_TYPE_I:
                        sprintf(pictype_str, "I");
                        break;
                    case AV_PICTURE_TYPE_P:
                        sprintf(pictype_str, "P");
                        break;
                    case AV_PICTURE_TYPE_B:
                        sprintf(pictype_str, "B");
                        break;
                    default:
                        sprintf(pictype_str, "Other");
                        break;
                }
                LOG_D(DEBUG,"Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
                frame_cnt++;
            }
        }
        av_free_packet(packet);
    }
    //flush decoder
    //FIX: Flush Frames remained in Codec
    while (1) {
        ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
        if (ret < 0)
            break;
        if (!got_picture)
            break;
        sws_scale(img_convert_ctx, (const uint8_t *const *) pFrame->data, pFrame->linesize, 0,
                  pCodecCtx->height,
                  pFrameYUV->data, pFrameYUV->linesize);
        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U
        fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V
        //Output info
        char pictype_str[10] = {0};
        switch (pFrame->pict_type) {
            case AV_PICTURE_TYPE_I:
                sprintf(pictype_str, "I");
                break;
            case AV_PICTURE_TYPE_P:
                sprintf(pictype_str, "P");
                break;
            case AV_PICTURE_TYPE_B:
                sprintf(pictype_str, "B");
                break;
            default:
                sprintf(pictype_str, "Other");
                break;
        }
        LOG_D(DEBUG,"Frame Index: %5d. Type:%s", frame_cnt, pictype_str);
        frame_cnt++;
    }
    time_finish = clock();
    time_duration = (double) (time_finish - time_start);

    sprintf(info, "%s[Time      ]%fms\n", info, time_duration);
    sprintf(info, "%s[Count     ]%d\n", info, frame_cnt);

    sws_freeContext(img_convert_ctx);

    fclose(fp_yuv);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    // TODO

    env->ReleaseStringUTFChars(inputUri_, inputUri);
    env->ReleaseStringUTFChars(outputUri_, outputUri);
    return 0;
}


JNIEXPORT jint JNICALL Java_com_guagua_nativeapp_MainActivity_nativePlayer(JNIEnv *env,
                                                                           jobject instance,
                                                                           jstring playUri_,
                                                                           jobject surface) {
    const char *playUri = env->GetStringUTFChars(playUri_, 0);
    LOG_D(DEBUG,"play");
    // sd卡中的视频文件地址,可自行修改或者通过jni传入
    const char *file_name = env->GetStringUTFChars(playUri_, 0);

    av_register_all();
    avformat_network_init();
    AVFormatContext *pFormatCtx = avformat_alloc_context();

    // Open video file
    if (avformat_open_input(&pFormatCtx, file_name, NULL, NULL) != 0) {
        LOG_E(DEBUG,"Couldn't open file:%s\n", file_name);
        return -1; // Couldn't open file
    }

    // Retrieve stream information
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        LOG_E(DEBUG,"Couldn't find stream information.");
        return -1;
    }

    // Find the first video stream
    int videoStream = -1, i;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO
            && videoStream < 0) {
            videoStream = i;
        }
    }
    if (videoStream == -1) {
        LOG_E(DEBUG,"Didn't find a video stream.");
        return -1; // Didn't find a video stream
    }
    // Get a pointer to the codec context for the video stream
    AVCodecContext *pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    // Find the decoder for the video stream
    AVCodec *pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
    if (pCodec == NULL) {
        LOG_E(DEBUG,"Codec not found.");
        return -1; // Codec not found
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOG_E(DEBUG,"Could not open codec.");
        return -1; // Could not open codec
    }

    // 获取native window
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        LOG_E(DEBUG,"Could not open codec.");
        return -1; // Could not open codec
    }

    // Allocate video frame
    AVFrame *pFrame = av_frame_alloc();

    // 用于渲染
    AVFrame *pFrameRGBA = av_frame_alloc();
    if (pFrameRGBA == NULL || pFrame == NULL) {
        LOG_E(DEBUG,"Could not allocate video frame.");
        return -1;
    }

    // Determine required buffer size and allocate buffer
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width,
                                            pCodecCtx->height, 1);
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    int frameFinished;
    AVPacket packet;
    while (av_read_frame(pFormatCtx, &packet) >= 0) {
        // Is this a packet from the video stream?
        if (packet.stream_index == videoStream) {

            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // 并不是decode一次就可解码出一帧
            if (frameFinished) {

                // lock native window buffer
                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 格式转换
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = (uint8_t *) (pFrameRGBA->data[0]);
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }

        }
        av_packet_unref(&packet);
    }

    av_free(buffer);
    av_free(pFrameRGBA);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codecs
    avcodec_close(pCodecCtx);

    // Close the video file
    avformat_close_input(&pFormatCtx);

    env->ReleaseStringUTFChars(playUri_, playUri);
    return 0;
}

JNIEXPORT jstring JNICALL
Java_com_guagua_nativeapp_YUVActivity_getYuvInfo(JNIEnv *env, jobject instance, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int video_index = -1, audio_index = -1;
    av_register_all();

    AVFormatContext *pFormatContext = avformat_alloc_context();

    if (avformat_open_input(&pFormatContext, path, NULL, NULL) !=0) {
        return env->NewStringUTF("");
    }

    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        return env->NewStringUTF("");
    }

    for (int i = 0; i < pFormatContext->nb_streams; i++) {
        if (pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
        } else if (pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;
        }
    }
    AVCodecContext *pVideoAVCodecContext = pFormatContext->streams[video_index]->codec;
    AVCodecContext *pAudioAVCodecContext = pFormatContext->streams[audio_index]->codec;

    AVCodec *pVideoCodec = avcodec_find_decoder(pVideoAVCodecContext->codec_id);
    AVCodec *pAudioCodec = avcodec_find_decoder(pAudioAVCodecContext->codec_id);
    LOG_D("video codec:%s,audio codec:%s",pVideoCodec->name,pAudioCodec->name);



    // TODO

    env->ReleaseStringUTFChars(path_, path);

//    avformat_free_context(pContext);

    return env->NewStringUTF("");
}


#ifdef __cplusplus
}
#endif