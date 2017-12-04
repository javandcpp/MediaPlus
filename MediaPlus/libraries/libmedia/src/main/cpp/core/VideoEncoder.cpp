//
// Created by developer on 11/13/17.
//


#include "VideoEncoder.h"
#include "StringUtils.h"
#include "CombineVideoHelper.h"
#include <stdio.h>


VideoEncoder *VideoEncoder::Get() {
    static VideoEncoder videoEncoder;
    return &videoEncoder;
}

VideoEncoder::VideoEncoder() {

}


extern "C" int64_t calc_dynamic_bitrate(int quality, int width, int height, int fps) {
    static const int BasicRate[4] = {240, 400, 500, 700};

    int64_t bit_rate = 0;
    double factor = sqrt((double) (width * height * fps) / (360 * 640 * 10.0));

    if (quality <= 85) {
        //10KB
        bit_rate = (int64_t) (BasicRate[0] * factor * quality / 85 + 0.5);
    } else if (quality <= 100) {
        //16KB
        int BitrateTemp = BasicRate[0] + ((BasicRate[1] - BasicRate[0]) * (quality - 85) / 15.0);
        bit_rate = (int64_t) (BitrateTemp * factor + 0.5);
    } else if (quality <= 125) {
        //28KB
        int BitrateTemp = BasicRate[1] + ((BasicRate[2] - BasicRate[1]) * (quality - 100) / 25.0);
        bit_rate = (int64_t) (BitrateTemp * factor + 0.5);
    } else if (quality <= 150) {
        //40KB
        int BitrateTemp = BasicRate[2] + ((BasicRate[3] - BasicRate[2]) * (quality - 125) / 25.0);
        bit_rate = (int64_t) (BitrateTemp * factor + 0.5);
    } else {
        int BitrateTemp = BasicRate[3] + 16 * (quality - 150);
        bit_rate = (int64_t) (BitrateTemp * factor + 0.5);
    }
    bit_rate *= 1000;

    return bit_rate;
}

VideoEncoder::~VideoEncoder() {
    if (NULL != videoCodecContext) {
        avcodec_free_context(&videoCodecContext);
        videoCodecContext = NULL;
    }
    if (nullptr != buffersrc) {
        av_free(buffersrc);
        buffersrc = nullptr;
    }
    if (nullptr != buffersink) {
        av_free(buffersink);
        buffersink = nullptr;
    }

    if (nullptr != inputs) {
        avfilter_inout_free(&inputs);
        inputs = nullptr;
    }
    if (nullptr != outputs) {
        avfilter_inout_free(&outputs);
        outputs = nullptr;
    }
    if (nullptr != drawTextFilter) {
        delete drawTextFilter;
    }

    LOG_D(DEBUG, "delete VideoEncoder");
}

void VideoEncoder::SetVideoCapture(VideoCapture *videoCapture) {
    this->videoCapture = videoCapture;
}

VideoCapture *VideoEncoder::GetVideoCapture() {
    return this->videoCapture;
}

void VideoEncoder::YUVProcessMirror() {

    return;
}

int VideoEncoder::EncodeH264(OriginData **originData) {
    if(GetVideoCapture()->enableWaterMark){
        CombineVideoHelper::Instance()->CombineWaterMark(0,(*originData)->data);
    }

    av_image_fill_arrays(outputYUVFrame->data,
                         outputYUVFrame->linesize, (*originData)->data,
                         AV_PIX_FMT_YUV420P, videoCodecContext->width,
                         videoCodecContext->height, 1);
    //文字添加


    outputYUVFrame->pts = (*originData)->pts;
    int ret = 0;
    ret = avcodec_send_frame(videoCodecContext, outputYUVFrame);
    if (ret != 0) {
#ifdef SHOW_DEBUG_INFO
        LOG_D(DEBUG, "avcodec video send frame failed");
#endif
    }
    av_packet_unref(&videoPacket);
    ret = avcodec_receive_packet(videoCodecContext, &videoPacket);
    if (ret != 0) {
#ifdef SHOW_DEBUG_INFO
        LOG_D(DEBUG, "avcodec video recieve packet failed");
#endif
    }
    (*originData)->Drop();
    (*originData)->avPacket = &videoPacket;
#ifdef SHOW_DEBUG_INFO
    LOG_D(DEBUG, "encode video packet size:%d   pts:%lld", (*originData)->avPacket->size,
          (*originData)->avPacket->pts);
    LOG_D(DEBUG, "Video frame encode success!");
#endif
    (*originData)->avPacket->size;
    return videoPacket.size;
}


void *VideoEncoder::EncodeTask(void *obj) {
    VideoEncoder *videoEncoder = (VideoEncoder *) obj;
    videoEncoder->isEncoding = true;
    VideoEncodeArgs *args = videoEncoder->videoCapture->GetVideoEncodeArgs();
    int totalSize = args->out_width * args->out_height * 3 / 2;
    int ySize = args->out_width * args->out_height;
    LOG_D(DEBUG, "in_height %d,int_width %d", args->out_height, args->out_width);

#ifdef WRITE_YUV_TO_FILE
    FILE *I420 = fopen("/mnt/sdcard/ivideo.yuv", "wb+");
#endif

    videoEncoder->videoCapture->videoCaputureframeQueue.clear();
    int64_t beginTime = av_gettime();
    int lastPts = 0;
    LOG_D(DEBUG, "begin video encode");
    int vpts = 0;
    while (true) {
        if (videoEncoder->videoCapture->GetCaptureState()) {
            break;
        }
        if (videoEncoder->videoCapture->videoCaputureframeQueue.empty()) {
            continue;
        }
        OriginData *srcData = videoEncoder->videoCapture->GetVideoData();
        if (srcData == NULL || srcData->size <= 0 || !srcData->data) {
            continue;
        }

        /**
         * 方式一：
         * I420手动填充AVFrame,需要注意ySize =width*height;
         */
        memcpy(videoEncoder->outputYUVFrame->data[0], srcData->data, ySize);//Y
        memcpy(videoEncoder->outputYUVFrame->data[1], srcData->data + ySize, ySize / 4);//U
        memcpy(videoEncoder->outputYUVFrame->data[2], srcData->data + (ySize * 5 / 4),
               ySize / 4);
        videoEncoder->outputYUVFrame->linesize[0] = videoEncoder->videoCodecContext->width;
        videoEncoder->outputYUVFrame->linesize[1] = videoEncoder->videoCodecContext->width / 2;
        videoEncoder->outputYUVFrame->linesize[2] = videoEncoder->videoCodecContext->width / 2;

        /**
         * 方式二:  deprecated
         */

//        avpicture_fill((AVPicture *) videoEncoder->outputYUVFrame, srcData->data, AV_PIX_FMT_YUV420P, videoEncoder->videoCodecContext->width,
//                       videoEncoder->videoCodecContext->height);

        /**
         * 方式三:
         */
//        av_image_fill_arrays(videoEncoder->outputYUVFrame->data,
//                             videoEncoder->outputYUVFrame->linesize, srcData->data,
//                             AV_PIX_FMT_YUV420P, videoEncoder->videoCodecContext->width,
//                             videoEncoder->videoCodecContext->height, 1);
#ifdef ENCODE_INFO
        LOG_D(DEBUG, "linesize[0]=%d", videoEncoder->outputYUVFrame->linesize[0]);
        LOG_D(DEBUG, "linesize[1]=%d", videoEncoder->outputYUVFrame->linesize[1]);
        LOG_D(DEBUG, "linesize[2]=%d", videoEncoder->outputYUVFrame->linesize[2]);
#endif
#ifdef PTS_INFO
        videoEncoder->outputYUVFrame->pts = srcData->pts - beginTime;
        LOG_D(DEBUG, "video pts:%lld", videoEncoder->outputYUVFrame->pts);
#endif
        //编码
        videoEncoder->outputYUVFrame->pts = srcData->pts - beginTime;
//        videoEncoder->outputYUVFrame->pts = vpts;
//        vpts++;

        int ret = 0;
        ret = avcodec_send_frame(videoEncoder->videoCodecContext, videoEncoder->outputYUVFrame);
        if (ret != 0) {
#ifdef SHOW_DEBUG_INFO
            LOG_D(DEBUG, "avcodec video send frame failed");
#endif
            continue;
        }
        av_packet_unref(&videoEncoder->videoPacket);
        ret = avcodec_receive_packet(videoEncoder->videoCodecContext, &videoEncoder->videoPacket);
        if (ret != 0) {
#ifdef SHOW_DEBUG_INFO
            LOG_D(DEBUG, "avcodec video recieve packet failed");
#endif
            continue;
        }

        srcData->Drop();
        //TODO 编码完成(数据传递问题不知道有没有问题)
        srcData->avPacket = &videoEncoder->videoPacket;
        srcData->pts = videoEncoder->outputYUVFrame->pts;
#ifdef SHOW_DEBUG_INFO
        LOG_D(DEBUG, "encode video packet size:%d   pts:%lld", srcData->avPacket->size,
              srcData->avPacket->pts);
        LOG_D(DEBUG, "Video frame encode success!");
#endif
        videoEncoder->vframeQueue.push(srcData);


#ifdef WRITE_YUV_TO_FILE
        fwrite(videoEncoder->outputYUVFrame->data[0], 1, ySize, I420);//Y
        fwrite(videoEncoder->outputYUVFrame->data[1], 1, ySize / 4, I420);//U
        fwrite(videoEncoder->outputYUVFrame->data[2], 1, ySize / 4, I420);//V
        fflush(I420);
#endif
    }

#ifdef WRITE_YUV_TO_FILE
    fclose(I420);
#endif


    return 0;
}

int VideoEncoder::StartEncode() {
    pthread_t t1;
    pthread_create(&t1, NULL, VideoEncoder::EncodeTask, this);
    LOG_D(DEBUG, "Start Video Encode task!")
    return 0;
}

int VideoEncoder::InitEncode() {
    int ret = 0;
    //find libfdk_aac avcodec
    avCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!avCodec) {
        LOG_D(DEBUG, "avcodec not found!");
        return -1;
    }
    videoCodecContext = avcodec_alloc_context3(avCodec);
    if (!videoCodecContext) {
        LOG_D(DEBUG, "avcodec alloc context failed!");
        return -1;
    }
    LOG_D(DEBUG, "avcodec alloc context success!");


    if (NULL != videoCapture->GetVideoEncodeArgs()) {
        videoCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; //全局参数
        videoCodecContext->codec_id = avCodec->id;
        videoCodecContext->bit_rate = 100 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
        videoCodecContext->width = videoCapture->GetVideoEncodeArgs()->out_height;
        videoCodecContext->height = videoCapture->GetVideoEncodeArgs()->out_width;
        videoCodecContext->framerate = {videoCapture->GetVideoEncodeArgs()->fps, 1};
        videoCodecContext->gop_size = 50;
        videoCodecContext->max_b_frames = 0;
        videoCodecContext->qmin = 10;
        videoCodecContext->qmax = 50;
        videoCodecContext->time_base = {1, 1000000};//AUDIO VIDEO 两边时间基数要相同
        videoCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

        videoCodecContext->level = 41;
        videoCodecContext->me_method = ME_HEX;
        videoCodecContext->refs = 1;
        videoCodecContext->chromaoffset = 2;
    }

    /**
     *选项: ultrafast, superfast, veryfast, faster, fast, medium, slow, slower, veryslow and placebo.
     * 固定码率150K,
     * 设置slow时：编码速度：245 fps 4ms
     * medium 时：编码速度：335 fps 2ms,
     * veryslow 时：编码速度：140 fps 7ms
     */

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "preset", "ultrafast", 0);//编码器的速度会影响推流音视频同步,所以这里需要设置下
//    av_dict_set(&opts, "tune", "zerolatency", 0);//如果开0延迟可能会影响视频质量
    av_dict_set(&opts, "profile", "baseline", 0);//I/P帧

    outputYUVFrame = av_frame_alloc();
    outputYUVFrame->format = AV_PIX_FMT_YUV420P;
    outputYUVFrame->width = videoCodecContext->width;//480
    outputYUVFrame->height = videoCodecContext->height;//640

    //分配yuv空间
    ret = av_frame_get_buffer(outputYUVFrame, 32);
    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf) - 1);
        LOG_E(DEBUG, "input av_frame_get_buffer:%s", buf);
        return -1;
    }


    ret = avcodec_open2(videoCodecContext, avCodec, &opts);
    //avcodec open failed! info: Invalid argument
    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "avcodec open failed! info: %s", buf);
        return -1;
    }
    LOG_D(DEBUG, "open video avcodec success!");

    LOG_D(DEBUG, "Complete Init Video Encode!")
    return 0;
}

int VideoEncoder::Release() {
    LOG_D(DEBUG, "Release Video Encode!")
    return 0;
}

bool VideoEncoder::GetEncodeState() {
    return isEncoding;
}

int VideoEncoder::CloseEncode() {
    std::lock_guard<std::mutex> lk(mut);
    if (isEncoding) {
        isEncoding = false;
        avcodec_close(videoCodecContext);
        LOG_D(DEBUG, "Close Video Encode!")
    }
    return 0;
}

int VideoEncoder::SetFilter(DrawTextFilter *drawTextFilter) {
    this->drawTextFilter = &drawTextFilter;
    return InitFilter();
}

/**
 * 初始化过滤器
 */
int VideoEncoder::InitFilter() {
    char args[512] = {0};
    //注意必须要调用avfilter_register_all()  否则avfilter_graph_create_filter  return -12
    buffersrc = avfilter_get_by_name("buffer");
    buffersink = avfilter_get_by_name("buffersink");
    outputs = avfilter_inout_alloc();
    inputs = avfilter_inout_alloc();
    int ret = 0;
    LOG_D(DEBUG, "this draw text filter font:%s context:%s x=%d,y=%d",
          (*drawTextFilter)->fontFilePath, (*drawTextFilter)->mContext,
          *(*drawTextFilter)->x, *(*drawTextFilter)->y);
//
//    char filters_descr[500] = {0};
//    const char *filters_descr = "boxblur";
    const char *filters_descr = "movie=/mnt/sccard/my_logo.png[wm];[in][wm]overlay=5:5[out]";
//    const char* filters_descr = "drawtext=fontfile=/mnt/sdcard/font1.ttf:text=hello world";

//    const char *filters_descr = "drawtext=fontfile=arial.ttf:fontcolor=green:fontsize=30:text='Lei Xiaohua'";
//    snprintf(filters_descr, sizeof(filters_descr),
//             "drawtext=fontfile=%s:fontsize=30:text=%s",
//             (*drawTextFilter)->fontFilePath, (*drawTextFilter)->mContext);
//
    LOG_D(DEBUG, "filter text:%s", filters_descr);

    enum AVPixelFormat pix_fmts[] = {AV_PIX_FMT_YUV420P, AV_PIX_FMT_YUV420P};
    filter_graph = avfilter_graph_alloc();
    if (!outputs || !inputs || !filter_graph) {
        LOG_D(DEBUG, "avfilter_graph_alloc failed!");
        return -1;
    }


    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             videoCodecContext->width, videoCodecContext->height, videoCodecContext->pix_fmt,
             videoCodecContext->time_base.num, videoCodecContext->time_base.den,
             videoCodecContext->sample_aspect_ratio.num, videoCodecContext->sample_aspect_ratio.den
    );


    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0) {
        char buf[102] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "Cannot create buffer source\n info:%s", buf);
        goto end;
    }

    AVBufferSinkParams *buffersink_params;
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0) {
        LOG_D(DEBUG, "Cannot create buffer sink\n");
        goto end;
    }


//    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
//                              AV_PIX_FMT_YUV420P, AV_OPT_SEARCH_CHILDREN);
//    if (ret < 0) {
//        LOG_D(DEBUG, "Cannot set output pixel format\n");
//        goto end;
//    }

    outputs->name = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = NULL;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filters_descr,
                                        &inputs, &outputs, NULL)) < 0) {
        char buf[102] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "avfilter_graph_parse_ptr failed info:%s", buf);
        goto end;
    }

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        LOG_D(DEBUG, "avfilter_graph_config failed");
        goto end;

    }
    LOG_D(DEBUG, "init filter success");
    return ret;

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);
    if (inputs != nullptr) {
        inputs = nullptr;
    }
    if (outputs != nullptr) {
        outputs = nullptr;
    }

    return ret;

}
