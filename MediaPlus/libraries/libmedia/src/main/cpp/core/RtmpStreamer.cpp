//
// Created by developer on 11/14/17.
//

#include "RtmpStreamer.h"

RtmpStreamer::RtmpStreamer() {

}

RtmpStreamer::~RtmpStreamer() {
    if (NULL != audioStream) {
        av_free(audioStream);
        audioStream = NULL;
    }
    if (NULL != videoStream) {
        av_free(videoStream);
        videoStream = NULL;
    }
    if (NULL != audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
        audioCodecContext = NULL;
    }
    if (NULL != videoCodecContext) {
        avcodec_free_context(&videoCodecContext);
        audioCodecContext = NULL;
    }
    if (NULL != iAvFormatContext) {
        avformat_free_context(iAvFormatContext);
        iAvFormatContext = NULL;
    }
}

RtmpStreamer *RtmpStreamer::Get() {
    static RtmpStreamer rtmpStreamer;
    return &rtmpStreamer;
}

int RtmpStreamer::InitStreamer(const char *url) {
    std::lock_guard<std::mutex> lk(mut);
    this->outputUrl = url;
    int ret = 0;
    ret = avformat_alloc_output_context2(&iAvFormatContext, NULL, "flv", outputUrl);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "avformat alloc output context2 failed: %s", buf);

        return -1;
    }
    LOG_D(DEBUG, "Rtmp InitStreamer Success!");
    return 0;
}

int RtmpStreamer::SetAudioEncoder(AudioEncoder *audioEncoder) {
    this->audioEncoder = audioEncoder;
    return this->audioEncoder != NULL ? 0 : -1;
}

int RtmpStreamer::SetVideoEncoder(VideoEncoder *videoEncoder) {
    this->videoEncoder = videoEncoder;
    return this->videoEncoder != NULL ? 0 : -1;
}

int RtmpStreamer::AddStream(AVCodecContext *avCodecContext) {
    std::lock_guard<std::mutex> lk(mut);
    AVStream *pStream = avformat_new_stream(iAvFormatContext, avCodecContext->codec);
    if (!pStream) {
        LOG_D(DEBUG, "avformat_new_stream failed!");
        return -1;
    }
    LOG_D(DEBUG, "avformat new stream success!");
    int ret = 0;
    ret = avcodec_parameters_from_context(pStream->codecpar, avCodecContext);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "avcodec_parameters_from_context failed :%s", buf);
        return -1;
    }
    LOG_D(DEBUG, "avcodec_parameters_from_context success!");
    pStream->codecpar->codec_tag = 0;
    if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        LOG_D(DEBUG, "Add video stream success!");
        videoStream = pStream;
        videoCodecContext = avCodecContext;
    } else if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        LOG_D(DEBUG, "Add audio stream success!");
        audioStream = pStream;
        audioCodecContext = avCodecContext;
    }
    return pStream->index;
}


int RtmpStreamer::SendAudioFrame(OriginData *originData, int streamIndex) {
    std::lock_guard<std::mutex> lk(mut1);
    AVRational stime;
    AVRational dtime;

    AVPacket *packet = originData->avPacket;
    packet->stream_index = streamIndex;
#ifdef SHOW_DEBUG_INFO
    LOG_D(DEBUG, "packet index:%d    index:%d", packet->stream_index, streamIndex);
#endif
    stime = audioCodecContext->time_base;
    dtime = audioStream->time_base;
    //push
    packet->pts = av_rescale_q(packet->pts, stime, dtime);
    packet->dts = av_rescale_q(packet->dts, stime, dtime);
    packet->duration = av_rescale_q(packet->duration, stime, dtime);
#ifdef SHOW_DEBUG_INFO
    LOG_D(DEBUG, "writer frame stream Index:%d   size:%d",
          packet->stream_index,
          packet->size);
#endif
    int ret = av_interleaved_write_frame(iAvFormatContext, packet);
    if (ret == 0) {
        LOG_D(DEBUG, "write ++++++++++++++audio frame sucess!");
    } else {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "writer******************* audio frame failed! :%s", buf);
    }
    delete originData;
    return 0;

}

int RtmpStreamer::SendVideoFrame(OriginData *originData, int streamIndex) {
    std::lock_guard<std::mutex> lk(mut1);
    AVRational stime;
    AVRational dtime;

    AVPacket *packet = originData->avPacket;
    packet->stream_index = streamIndex;
#ifdef SHOW_DEBUG_INFO
    LOG_D(DEBUG, "video packet index:%d    index:%d", packet->stream_index, streamIndex);
#endif
    stime = videoCodecContext->time_base;
    dtime = videoStream->time_base;
    packet->pts = originData->pts;
    packet->dts = packet->pts;
    packet->pts = av_rescale_q_rnd(packet->pts, stime, dtime,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->dts = av_rescale_q_rnd(packet->dts, stime, dtime,
                                   (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    packet->duration = av_rescale_q(packet->duration, stime, dtime);
#ifdef SHOW_DEBUG_INFO
    LOG_D(DEBUG, "writer frame stream Index:%d   size:%d",
          packet->stream_index,
          packet->size);

#endif
    int ret = av_interleaved_write_frame(iAvFormatContext, packet);
    if (ret == 0) {
        LOG_D(DEBUG, "write ------------------video frame success!");
    } else {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "writer*******************video frame failed! :%s", buf);
    }
    delete originData;
    return 0;

}


/**
 * 音频推流任务
 */
void *RtmpStreamer::PushAudioStreamTask(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;
    rtmpStreamer->isPushStream = true;

    if (NULL == rtmpStreamer->audioEncoder) {
        return 0;
    }
    AudioCapture *pAudioCapture = rtmpStreamer->audioEncoder->GetAudioCapture();
//    VideoCapture *pVideoCapture = rtmpStreamer->videoEncoder->GetVideoCapture();

    if (NULL == pAudioCapture) {
        return 0;
    }
    int64_t beginTime = av_gettime();
    int64_t lastAudioPts = 0;
    while (true) {

        if (!rtmpStreamer->isPushStream ||
            pAudioCapture->GetCaptureState()) {
            break;
        }
        OriginData *pAudioData = pAudioCapture->GetAudioData();
//        OriginData *pVideoData = pVideoCapture->GetVideoData();

        if (pAudioData != NULL && pAudioData->data) {
//            if(pVideoData!=NULL&&pAudioData->pts<pVideoData->pts){
//                int64_t subPtsValue = pVideoData->pts - pAudioData->pts;
//                pAudioData->pts+=subPtsValue;
//            }
            pAudioData->pts = pAudioData->pts - beginTime;
//            if (pAudioData->pts == lastAudioPts) {
//                pAudioData->pts += 1300;
//            }
//            lastAudioPts = pAudioData->pts;
            LOG_D(DEBUG, "before audio encode pts:%lld", pAudioData->pts);
            rtmpStreamer->audioEncoder->EncodeAAC(&pAudioData);
            LOG_D(DEBUG, "after audio encode pts:%lld", pAudioData->avPacket->pts);
        }
        if (pAudioData != NULL && pAudioData->avPacket->size > 0) {
            rtmpStreamer->SendFrame(pAudioData, rtmpStreamer->audioStreamIndex);
        }
    }

    return 0;
}

/**
* 音频推流任务
*/
void *RtmpStreamer::PushVideoStreamTask(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;
    rtmpStreamer->isPushStream = true;

    if (NULL == rtmpStreamer->videoEncoder) {
        return 0;
    }
    VideoCapture *pVideoCapture = rtmpStreamer->videoEncoder->GetVideoCapture();
    AudioCapture *pAudioCapture = rtmpStreamer->audioEncoder->GetAudioCapture();

    if (NULL == pVideoCapture) {
        return 0;
    }
    int64_t beginTime = av_gettime();
    int64_t lastAudioPts = 0;
    while (true) {

        if (!rtmpStreamer->isPushStream ||
            pVideoCapture->GetCaptureState()) {
            break;
        }
        
        OriginData *pVideoData = pVideoCapture->GetVideoData();
//        OriginData *pAudioData = pAudioCapture->GetAudioData();
        //h264 encode
        if (pVideoData != NULL && pVideoData->data) {
//            if(pAudioData&&pAudioData->pts>pVideoData->pts){
//                int64_t overValue=pAudioData->pts-pVideoData->pts;
//                pVideoData->pts+=overValue+1000;
//                LOG_D(DEBUG, "synchronized video audio pts  videoPts:%lld   audioPts:%lld", pVideoData->pts,pAudioData->pts);
//            }
            pVideoData->pts = pVideoData->pts - beginTime;
            LOG_D(DEBUG, "before video encode pts:%lld", pVideoData->pts);
            rtmpStreamer->videoEncoder->EncodeH264(&pVideoData);
            LOG_D(DEBUG, "after video encode pts:%lld", pVideoData->avPacket->pts);
        }

        if (pVideoData != NULL && pVideoData->avPacket->size > 0) {
            rtmpStreamer->SendFrame(pVideoData, rtmpStreamer->videoStreamIndex);
        }
    }
    return 0;
}

/**
* 音视频同时推流任务
*/
void *RtmpStreamer::PushStreamTask(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;
    rtmpStreamer->isPushStream = true;

    if (NULL == rtmpStreamer->videoEncoder || NULL == rtmpStreamer->audioEncoder) {
        return 0;
    }
    VideoCapture *pVideoCapture = rtmpStreamer->videoEncoder->GetVideoCapture();
    AudioCapture *pAudioCapture = rtmpStreamer->audioEncoder->GetAudioCapture();

    if (NULL == pVideoCapture || NULL == pAudioCapture) {
        return 0;
    }
    int64_t beginTime = av_gettime();
    if (NULL != pVideoCapture) {
        pVideoCapture->videoCaputureframeQueue.clear();
    }
    if (NULL != pAudioCapture) {
        pAudioCapture->audioCaputureframeQueue.clear();
    }
    int64_t lastAudioPts = 0;
    while (true) {

        if (!rtmpStreamer->isPushStream ||
            pVideoCapture->GetCaptureState() ||
            pAudioCapture->GetCaptureState()) {
            break;
        }
        OriginData *pVideoData = pVideoCapture->GetVideoData();
        OriginData *pAudioData = pAudioCapture->GetAudioData();

        if (pAudioData != NULL && pAudioData->data) {
            pAudioData->pts = pAudioData->pts - beginTime;
//            if (pAudioData->pts == lastAudioPts) {
//                pAudioData->pts += 1300;
//            }
            lastAudioPts = pAudioData->pts;
            LOG_D(DEBUG, "before audio encode pts:%lld", pAudioData->pts);
            rtmpStreamer->audioEncoder->EncodeAAC(&pAudioData);
            LOG_D(DEBUG, "after audio encode pts:%lld", pAudioData->avPacket->pts);
        }


        if (pAudioData != NULL && pAudioData->avPacket->size > 0) {
            rtmpStreamer->SendFrame(pAudioData, rtmpStreamer->audioStreamIndex);
        }

        //h264 encode
        if (pVideoData != NULL && pVideoData->data) {
            pVideoData->pts = pVideoData->pts - beginTime;
            LOG_D(DEBUG, "before video encode pts:%lld", pVideoData->pts);
            rtmpStreamer->videoEncoder->EncodeH264(&pVideoData);
            LOG_D(DEBUG, "after video encode pts:%lld", pVideoData->avPacket->pts);
        }

        if (pVideoData != NULL && pVideoData->avPacket->size > 0) {
            rtmpStreamer->SendFrame(pVideoData, rtmpStreamer->videoStreamIndex);
        }
    }
    return 0;
}

int RtmpStreamer::StartPushStream() {
    videoStreamIndex = AddStream(videoEncoder->videoCodecContext);
    audioStreamIndex = AddStream(audioEncoder->audioCodecContext);
    pthread_create(&t3, NULL, RtmpStreamer::WriteHead, this);
    pthread_join(t3, NULL);

    VideoCapture *pVideoCapture = videoEncoder->GetVideoCapture();
    AudioCapture *pAudioCapture = audioEncoder->GetAudioCapture();
    pVideoCapture->videoCaputureframeQueue.clear();
    pAudioCapture->audioCaputureframeQueue.clear();

    if(writeHeadFinish) {
        pthread_create(&t1, NULL, RtmpStreamer::PushAudioStreamTask, this);
        pthread_create(&t2, NULL, RtmpStreamer::PushVideoStreamTask, this);
    }else{
        return -1;
    }
//    pthread_create(&t2, NULL, RtmpStreamer::PushStreamTask, this);
//    pthread_create(&t2, NULL, RtmpStreamer::PushStreamTask, this);

    return 0;
}

int RtmpStreamer::ClosePushStream() {
    if (isPushStream) {
        isPushStream = false;
//        pthread_join(t1, NULL);
        pthread_join(t2, NULL);
        if (NULL != iAvFormatContext) {
            av_write_trailer(iAvFormatContext);
            avio_close(iAvFormatContext->pb);
        }
    }
    writeHeadFinish=false;
    return 0;
}

/**
 * notice:AVStream创建完成开始写头信息
 */
void *RtmpStreamer::WriteHead(void *pObj) {
    RtmpStreamer *rtmpStreamer = (RtmpStreamer *) pObj;
    int ret = 0;
    ret = avio_open(&rtmpStreamer->iAvFormatContext->pb, rtmpStreamer->outputUrl, AVIO_FLAG_WRITE);
    if (ret < 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "avio open failed: %s", buf);
        return 0;
    }
    LOG_D(DEBUG, "avio open success!");
    ret = avformat_write_header(rtmpStreamer->iAvFormatContext, NULL);
    if (ret != 0) {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "avformat write header failed!: %s", buf);
        return 0;
    }
    rtmpStreamer->writeHeadFinish=true;
    return 0;
}

int RtmpStreamer::SendFrame(OriginData *pData, int streamIndex) {
    std::lock_guard<std::mutex> lk(mut1);
    AVRational stime;
    AVRational dtime;
    AVPacket *packet = pData->avPacket;
    packet->stream_index = streamIndex;
    LOG_D(DEBUG, "write packet index:%d    index:%d   pts:%lld", packet->stream_index, streamIndex,
          packet->pts);
    //判断是音频还是视频
    if (packet->stream_index == videoStreamIndex) {
        stime = videoCodecContext->time_base;
        dtime = videoStream->time_base;
    }
    else if (packet->stream_index == audioStreamIndex) {
        stime = audioCodecContext->time_base;
        dtime = audioStream->time_base;
    }
    else {
        LOG_D(DEBUG, "unknow stream index");
        return -1;
    }
    packet->pts = av_rescale_q(packet->pts, stime, dtime);
    packet->dts = av_rescale_q(packet->dts, stime, dtime);
    packet->duration = av_rescale_q(packet->duration, stime, dtime);
    int ret = av_interleaved_write_frame(iAvFormatContext, packet);

    if (ret == 0) {
        if (streamIndex == audioStreamIndex) {
            LOG_D(DEBUG, "---------->write @@@@@@@@@ frame success------->!");
        } else if (streamIndex == videoStreamIndex) {
            LOG_D(DEBUG, "---------->write ######### frame success------->!");
        }
    } else {
        char buf[1024] = {0};
        av_strerror(ret, buf, sizeof(buf));
        LOG_D(DEBUG, "stream index %d writer frame failed! :%s", streamIndex, buf);
    }
    return 0;
}





