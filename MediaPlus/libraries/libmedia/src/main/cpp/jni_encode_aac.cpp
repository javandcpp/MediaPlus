//
// Created by developer on 8/18/17.
//

#include "jni_encode_aac.h"


/**
 * 构造函数接收传入参数
 */
AACEncode::AACEncode(UserArguments *arguments) : userArguments(arguments) { }

AACEncode::~AACEncode() {
    if (userArguments->audio_path != NULL) {
        free(userArguments->audio_path);
    }
    if (userArguments != NULL) {
        delete userArguments;
    }
}

/**
 *刷出编码器里剩余帧
 */
int AACEncode::flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
    int ret;
    int got_frame;
    AVPacket enc_pkt;
    if (!(fmt_ctx->streams[stream_index]->codec->codec->capabilities &
          CODEC_CAP_DELAY))
        return 0;
    while (1) {
        enc_pkt.data = NULL;
        enc_pkt.size = 0;
        av_init_packet(&enc_pkt);
        ret = avcodec_encode_audio2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
                                    NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        LOG_D(DEBUG, "Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n", enc_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }
    return ret;
}

/**
 * 初始化音频编码器
 * @return
 */
int AACEncode::initAudioEncoder() {
    LOG_I(DEBUG,"音频编码器初始化开始");
    size_t path_length = strlen(userArguments->audio_path);
    char *out_file=( char *)malloc(path_length+1);

    strcpy(out_file, userArguments->audio_path);

    av_register_all();

    //Method 1.
    pFormatCtx = avformat_alloc_context();
    fmt = av_guess_format(NULL, out_file, NULL);
    pFormatCtx->oformat = fmt;


//    Method 2.
//    int a=avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
//    fmt = pFormatCtx->oformat;
//    pCodec = avcodec_find_encoder(AV_CODEC_ID_AAC);
//  、  pCodecCtx = avcodec_alloc_context3(pCodec);

    //Open output URL
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        LOG_E(DEBUG,"Failed to open output file!\n");
        return -1;
    }
//    pFormatCtx->audio_codec_id=AV_CODEC_ID_AAC;

    audio_st = avformat_new_stream(pFormatCtx, 0);
    if (audio_st == NULL) {
        return -1;
    }
    pCodecCtx = audio_st->codec;
    pCodecCtx->codec_id = AV_CODEC_ID_AAC;
    pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    pCodecCtx->sample_rate = userArguments->audio_sample_rate;
    pCodecCtx->channel_layout = AV_CH_LAYOUT_MONO;
    pCodecCtx->channels = av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    pCodecCtx->bit_rate = userArguments->audio_bit_rate;
//    pCodecCtx->thread_count = 1;
//    pCodecCtx->profile=FF_PROFILE_AAC_MAIN;

    int b= av_get_channel_layout_nb_channels(pCodecCtx->channel_layout);
    LOG_I(DEBUG,"native --channels:%d",b);

    //Show some information
    av_dump_format(pFormatCtx, 0, out_file, 1);
    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    LOG_I(DEBUG,"natvie --avcodec_open:%s",pCodec->name);
    if (!pCodec) {

        LOG_E(DEBUG,"natvie --Can not find encoder!\n");
        return -1;
    }

//    AVDictionary *param = 0;
//
//    av_dict_set(&param, "profile", "aac_he", 0);


    int state = avcodec_open2(pCodecCtx, pCodec, NULL);
    LOG_I(DEBUG,"natvie --avcodec_open:%d",state);
    if (state < 0) {
        LOG_E(DEBUG,"natvie --Failed to open encoder!---%d",state);
        return -1;
    }
    pFrame = av_frame_alloc();
    pFrame->nb_samples = pCodecCtx->frame_size;
    pFrame->format = pCodecCtx->sample_fmt;

    size = av_samples_get_buffer_size(NULL, pCodecCtx->channels, pCodecCtx->frame_size,
                                      pCodecCtx->sample_fmt, 1);

    uint8_t *frame_buf = (uint8_t *) av_malloc(size);
    avcodec_fill_audio_frame(pFrame, pCodecCtx->channels, pCodecCtx->sample_fmt,
                             (const uint8_t *) frame_buf, size, 1);

    //Write Header
    avformat_write_header(pFormatCtx, NULL);

    av_new_packet(&pkt, size);
    is_end=START;
    pthread_t thread;
    pthread_create(&thread, NULL, AACEncode::startEncode, this);
    LOG_I(DEBUG,"natvie --音频编码器初始化完成");
    return 0;

}

/**
 * 用户结束标记
 */
void AACEncode::user_end() {
    is_end = END;
}

void AACEncode::release() {

    is_release = RELEASE;
}

int AACEncode::sendOneFrame(uint8_t *buf) {
    uint8_t *new_buf = (uint8_t *) malloc(size);
    memcpy(new_buf, buf, size);
    frame_queue.push(new_buf);
    return 0;
}

/**
 * 编码结束操作
 * @return
 */
int AACEncode::encodeEnd() {
    //Flush Encoder
    ret = flush_encoder(pFormatCtx, 0);
    if (ret < 0) {
        LOG_E(DEBUG, "Flushing encoder failed\n");
        return -1;
    }
    LOG_E(DEBUG, "Flushing encoder success\n");
    //Write Trailer
    av_write_trailer(pFormatCtx);

    //Clean
    LOG_E(DEBUG, "av_write_trailer\n");
    if (audio_st) {
        avcodec_close(audio_st->codec);
        av_free(pFrame);
        LOG_E(DEBUG, "av_free\n");
//        av_free(frame_buf);
    }
    avio_close(pFormatCtx->pb);
    LOG_E(DEBUG, "avio_close\n");
    avformat_free_context(pFormatCtx);
    LOG_I(DEBUG, "音频编码完成");
//    userArguments->handler->setup_audio_state(END_STATE);
//    arguments->handler->try_encode_over(arguments);

    return 0;
}

/**
 * 开启编码线程
 * @param obj
 * @return
 */
void *AACEncode::startEncode(void *obj) {
    AACEncode *aac_encoder = (AACEncode *) obj;
    while (!aac_encoder->is_end || !aac_encoder->frame_queue.empty()) {
        if (aac_encoder->is_release) {
            if (aac_encoder->audio_st) {
                avcodec_close(aac_encoder->audio_st->codec);
                av_free(aac_encoder->pFrame);
//        av_free(frame_buf);
            }
            avio_close(aac_encoder->pFormatCtx->pb);
            avformat_free_context(aac_encoder->pFormatCtx);
            return 0;
        }

        if (aac_encoder->frame_queue.empty()) {
            continue;
        }
        uint8_t *frame_buf = *aac_encoder->frame_queue.wait_and_pop().get();

        aac_encoder->pFrame->data[0] = frame_buf;

        aac_encoder->pFrame->pts = aac_encoder->i;
        aac_encoder->i++;
        aac_encoder->got_frame = 0;
        //Encode
        aac_encoder->ret = avcodec_encode_audio2(aac_encoder->pCodecCtx, &aac_encoder->pkt,
                                                 aac_encoder->pFrame, &aac_encoder->got_frame);
        if (aac_encoder->ret < 0) {
            LOG_E(DEBUG, "Failed to encode!\n");
        }

        if (aac_encoder->got_frame == 1) {
            LOG_I(DEBUG, "Succeed to encode 1 frame! \tsize:%5d\n", aac_encoder->pkt.size);
            aac_encoder->pkt.stream_index = aac_encoder->audio_st->index;
            aac_encoder->
                    ret = av_write_frame(aac_encoder->pFormatCtx, &aac_encoder->pkt);
            av_free_packet(&aac_encoder->pkt);
        }
        delete (frame_buf);
    }
    if (aac_encoder->is_end) {
        aac_encoder->encodeEnd();
        delete aac_encoder;
    }
    return 0;
}

