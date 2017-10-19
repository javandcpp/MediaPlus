//
// Created by developer on 8/17/17.
//


#include "jni_encode_h264.h"


int frame_index;

H264Encode::H264Encode(UserArguments *userArguments) : arguments(userArguments) {


}

H264Encode::~H264Encode() {

}

/**
 * 结束编码时刷出还在编码器里面的帧
 * @param fmt_ctx
 * @param stream_index
 * @return
 */
int flush_encoder(AVFormatContext *fmt_ctx, unsigned int stream_index) {
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
        ret = avcodec_encode_video2(fmt_ctx->streams[stream_index]->codec, &enc_pkt,
                                    NULL, &got_frame);
        av_frame_free(NULL);
        if (ret < 0)
            break;
        if (!got_frame) {
            ret = 0;
            break;
        }
        LOG_I(DEBUG, "_Flush Encoder: Succeed to encode 1 frame video!\tsize:%5d\n",
              enc_pkt.size);
        /* mux encoded frame */
        ret = av_write_frame(fmt_ctx, &enc_pkt);
        if (ret < 0)
            break;
    }

    return ret;
}

/**
 * 初始化视频编码器
 * @return
 */
int H264Encode::initVideoEncoder() {
    LOG_I(DEBUG, "视频编码器初始化开始");

    size_t path_length = strlen(arguments->video_path);
    char *out_file = (char *) malloc(path_length + 1);
    strcpy(out_file, arguments->video_path);

    av_register_all();//注册多种音视频格式的编解码器,并注册各种文件的编解复用器
    avformat_network_init();
    //Method1.
//    pFormatCtx = avformat_alloc_context();
//    //Guess Format
//    fmt = av_guess_format(NULL, out_file, NULL);
//
//    LOGE(JNI_DEBUG,",fmt==null?:%s", fmt == NULL ? "null" : "no_null");
//    pFormatCtx->oformat = fmt;

    //Method 2.
    //在基于FFmpeg的视音频编码器程序中,该函数通常是第一个调用的函数（除了组件注册函数av_register_all()）,初始化一个用于输出的AVFormatContext结构体.
    //第二个参数用于指定输出格式,如果指定为NULL，可以设定后两个参数（format_name或者filename）由FFmpeg猜测输出格式,函数执行成功的话，其返回值大于等于0。
    avformat_alloc_output_context2(&pFormatCtx, NULL, "flv", out_file);//rtmp推流
    fmt = pFormatCtx->oformat;

    //Open output URL
    if (avio_open(&pFormatCtx->pb, out_file, AVIO_FLAG_READ_WRITE) < 0) {
        LOG_E(DEBUG, "_Failed to open output file! \n");
        return -1;
    }

    video_st = avformat_new_stream(pFormatCtx, 0);
    //video_st->time_base.num = 1;
    //video_st->time_base.den = 25;

    if (video_st == NULL) {
        LOG_E(DEBUG, "_video_st==null");
        return -1;
    }

    //Param that must set
    pCodecCtx = video_st->codec;
    //pCodecCtx->codec_id =AV_CODEC_ID_HEVC;
    pCodecCtx->codec_id = AV_CODEC_ID_H264;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    if (arguments->v_custom_format == ROTATE_0_CROP_LT ||
        arguments->v_custom_format == ROTATE_180) {
        pCodecCtx->width = arguments->out_width;
        pCodecCtx->height = arguments->out_height;
    } else {
        pCodecCtx->width = arguments->out_height;
        pCodecCtx->height = arguments->out_width;
    }

    pCodecCtx->bit_rate = arguments->video_bit_rate;
    pCodecCtx->gop_size = 50;
    pCodecCtx->thread_count = 12;

    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = arguments->frame_rate;
//    pCodecCtx->me_pre_cmp = 1;
    //H264
    //pCodecCtx->me_range = 16;
    //pCodecCtx->max_qdiff = 4;
    //pCodecCtx->qcompress = 0.6;
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51;

    //Optional Param
    pCodecCtx->max_b_frames = 3;

    // Set Option
    AVDictionary *param = 0;
    //H.264
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264) {
//        av_dict_set(&param, "tune", "animation", 0);
//        av_dict_set(&param, "profile", "baseline", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
        av_opt_set(pCodecCtx->priv_data, "preset", "ultrafast", 0);
        av_dict_set(&param, "profile", "baseline", 0);
    }

    //Show some Information
    av_dump_format(pFormatCtx, 0, out_file, 1);//检查初始化参数是否符合规范

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec) {
        LOG_E(DEBUG, "Can not find encoder! \n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {
        LOG_E(DEBUG, "Failed to open encoder! \n");
        return -1;
    }


    pFrame = av_frame_alloc();
    picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    LOG_I(DEBUG, "   picture_size:%d", picture_size);
    uint8_t *buf = (uint8_t *) av_malloc(picture_size);
    avpicture_fill((AVPicture *) pFrame, buf, pCodecCtx->pix_fmt, pCodecCtx->width,
                   pCodecCtx->height);

    //Write File Header
    avformat_write_header(pFormatCtx, NULL);
    av_new_packet(&pkt, picture_size);
    out_y_size = pCodecCtx->width * pCodecCtx->height;
    is_end = START;
    pthread_t thread;
    pthread_create(&thread, NULL, H264Encode::startEncode, this);

    LOG_I(DEBUG, "视频编码器初始化完成");

    return 0;
}

/**
 * 发送一帧到编码队列
 * @param buf
 * @return
 */
int H264Encode::sendOneFrame(uint8_t *buf) {
    int in_y_size = arguments->in_width * arguments->in_height;

    uint8_t *new_buf = (uint8_t *) malloc(in_y_size * 3 / 2);
    memcpy(new_buf, buf, in_y_size * 3 / 2);

    frame_queue.push(new_buf);

    return 0;
}

/**
 * 启动编码线程
 * @param obj
 * @return
 */
void *H264Encode::startEncode(void *obj) {
    H264Encode *h264_encoder = (H264Encode *) obj;
    int64_t start_time = av_gettime();

    AVBitStreamFilterContext *h264bsfc=av_bitstream_filter_init("h264_mp4toannexb");

    while (!h264_encoder->is_end || !h264_encoder->frame_queue.empty()) {
        if (h264_encoder->is_release) {
            //Write file trailer
            av_write_trailer(h264_encoder->pFormatCtx);

            //Clean
            if (h264_encoder->video_st) {
                avcodec_close(h264_encoder->video_st->codec);
                av_free(h264_encoder->pFrame);
            }
            avio_close(h264_encoder->pFormatCtx->pb);
            avformat_free_context(h264_encoder->pFormatCtx);
            delete h264_encoder;
            return 0;
        }
        if (h264_encoder->frame_queue.empty()) {
            continue;
        }
        uint8_t *picture_buf = *h264_encoder->frame_queue.wait_and_pop().get();
        LOG_I(DEBUG, "send_videoframe_count:%d", h264_encoder->frame_count);
        int in_y_size = h264_encoder->arguments->in_width * h264_encoder->arguments->in_height;
        h264_encoder->custom_filter(h264_encoder, picture_buf, in_y_size,
                                    h264_encoder->arguments->v_custom_format);


//        h264_encoder->pFrame->data[0] = picture_buf;
//        h264_encoder->pFrame->data[2] = picture_buf + h264_encoder->out_y_size;
//        h264_encoder->pFrame->data[1] = picture_buf + h264_encoder->out_y_size * 5 / 4;
//    memcpy(h264_encoder->pFrame->data[0],picture_buf,h264_encoder->out_y_size);
//    memcpy(h264_encoder->pFrame->data[2],picture_buf+h264_encoder->out_y_size,h264_encoder->out_y_size/4);
//    memcpy(h264_encoder->pFrame->data[1],picture_buf+h264_encoder->out_y_size*5/4,h264_encoder->out_y_size/4);
        //PTS
        h264_encoder->pFrame->pts = h264_encoder->frame_count;
        h264_encoder->frame_count++;
        int got_picture = 0;
        //Encode
        int ret = avcodec_encode_video2(h264_encoder->pCodecCtx, &h264_encoder->pkt,
                                        h264_encoder->pFrame, &got_picture);
        if (ret < 0) {
            LOG_E(DEBUG, "Failed to encode! \n");
        }
        if (got_picture == 1) {//1表示成功编码一帧


            if (h264_encoder->pkt.pts == AV_NOPTS_VALUE) {
                //Write PTS
                AVRational time_base1 = AV_TIME_BASE_Q;
                //Duration between 2 frames (us)
                int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(time_base1);
                //Parameters
                h264_encoder->pkt.pts = (double) (frame_index * calc_duration) /
                                        (double) (av_q2d(time_base1) * AV_TIME_BASE);
                h264_encoder->pkt.dts = h264_encoder->pkt.pts;
                h264_encoder->pkt.duration =
                        (double) calc_duration / (double) (av_q2d(time_base1) * AV_TIME_BASE);
            }

            //Important:Delay
//            if(h264_encoder->pkt.stream_index==videoindex){
            AVRational time_base = AV_TIME_BASE_Q;
            AVRational time_base_q = {1, AV_TIME_BASE};
            int64_t pts_time = av_rescale_q(h264_encoder->pkt.dts, time_base, time_base_q);
            int64_t now_time = av_gettime() - start_time;
            if (pts_time > now_time)
                av_usleep(pts_time - now_time);

//            }

            h264_encoder->pkt.pts = av_rescale_q_rnd(h264_encoder->pkt.pts, AV_TIME_BASE_Q,
                                                     h264_encoder->video_st->time_base,
                                                     (AVRounding) (AV_ROUND_NEAR_INF |
                                                                   AV_ROUND_PASS_MINMAX));
            h264_encoder->pkt.dts = av_rescale_q_rnd(h264_encoder->pkt.dts, AV_TIME_BASE_Q,
                                                     h264_encoder->video_st->time_base,
                                                     (AVRounding) (AV_ROUND_NEAR_INF |
                                                                   AV_ROUND_PASS_MINMAX));
            h264_encoder->pkt.duration = av_rescale_q(h264_encoder->pkt.duration, AV_TIME_BASE_Q,
                                                      h264_encoder->video_st->time_base);
            h264_encoder->pkt.pos = -1;
            //Print to Screen
//            if(pkt.stream_index==videoindex){
            LOG_I(DEBUG, "Send %8d video frames to output URL\n", frame_index);
            frame_index++;
//            }


            LOG_I(DEBUG, "Succeed to encode frame: %5d\tsize:%5d\n", h264_encoder->framecnt,
                  h264_encoder->pkt.size);
            h264_encoder->framecnt++;
            h264_encoder->pkt.stream_index = h264_encoder->video_st->index;

            av_bitstream_filter_filter(h264bsfc,h264_encoder->pCodecCtx,NULL,&h264_encoder->pkt.data,&h264_encoder->pkt.size,h264_encoder->pkt.data,h264_encoder->pkt.size,0);

            ret = av_interleaved_write_frame(h264_encoder->pFormatCtx, &h264_encoder->pkt);


            if (ret < 0) {
                char *error=av_err2str(ret);
                LOG_E(DEBUG,"error msg:%s",error);
            }
            LOG_I(DEBUG, "av_write_frame:%d\n", ret);
            av_free_packet(&h264_encoder->pkt);
        }
        delete (picture_buf);
    }
    if (h264_encoder->is_end) {
        av_bitstream_filter_close(h264bsfc);
        h264_encoder->encodeEnd();
        delete h264_encoder;
    }
    return 0;
}

/**
 * 对视频做一些处理
 * @param h264_encoder
 * @param picture_buf
 * @param in_y_size
 * @param format
 */
void H264Encode::custom_filter(const H264Encode *h264_encoder, const uint8_t *picture_buf,
                               int in_y_size, int format) {



    //   y值在H方向开始行
    int y_height_start_index =
            h264_encoder->arguments->in_height - h264_encoder->arguments->out_height;
    //   uv值在H方向开始行
    int uv_height_start_index = y_height_start_index / 2;

    if (format == ROTATE_90_CROP_LT) {

        for (int i = y_height_start_index; i < h264_encoder->arguments->in_height; i++) {

            for (int j = 0; j < h264_encoder->arguments->out_width; j++) {

                int index = h264_encoder->arguments->in_width * i + j;
                uint8_t value = *(picture_buf + index);
                *(h264_encoder->pFrame->data[0] + j * h264_encoder->arguments->out_height +
                  (h264_encoder->arguments->out_height - (i - y_height_start_index) - 1)) = value;
            }
        }

        for (int i = uv_height_start_index; i < h264_encoder->arguments->in_height / 2; i++) {
            for (int j = 0; j < h264_encoder->arguments->out_width / 2; j++) {
                int index = h264_encoder->arguments->in_width / 2 * i + j;
                uint8_t v = *(picture_buf + in_y_size + index);
                uint8_t u = *(picture_buf + in_y_size * 5 / 4 + index);
                *(h264_encoder->pFrame->data[2] + (j * h264_encoder->arguments->out_height / 2 +
                                                   (h264_encoder->arguments->out_height / 2 -
                                                    (i - uv_height_start_index) -
                                                    1))) = v;
                *(h264_encoder->pFrame->data[1] + (j * h264_encoder->arguments->out_height / 2 +
                                                   (h264_encoder->arguments->out_height / 2 -
                                                    (i - uv_height_start_index) -
                                                    1))) = u;
            }
        }
    } else if (format == ROTATE_0_CROP_LT) {


        for (int i = y_height_start_index; i < h264_encoder->arguments->in_height; i++) {

            for (int j = 0; j < h264_encoder->arguments->out_width; j++) {

                int index = h264_encoder->arguments->in_width * i + j;
                uint8_t value = *(picture_buf + index);

                *(h264_encoder->pFrame->data[0] +
                  (i - y_height_start_index) * h264_encoder->arguments->out_width +
                  j) = value;
            }
        }


        for (int i = uv_height_start_index; i < h264_encoder->arguments->in_height / 2; i++) {
            for (int j = 0; j < h264_encoder->arguments->out_width / 2; j++) {

                int index = h264_encoder->arguments->in_width / 2 * i + j;
                uint8_t v = *(picture_buf + in_y_size + index);

                uint8_t u = *(picture_buf + in_y_size * 5 / 4 + index);
                *(h264_encoder->pFrame->data[2] +
                  ((i - uv_height_start_index) * h264_encoder->arguments->out_width / 2 + j)) = v;
                *(h264_encoder->pFrame->data[1] +
                  ((i - uv_height_start_index) * h264_encoder->arguments->out_width / 2 + j)) = u;
            }
        }
    } else if (format == ROTATE_270_CROP_LT_MIRROR_LR) {

        int y_width_start_index =
                h264_encoder->arguments->in_width - h264_encoder->arguments->out_width;
        int uv_width_start_index = y_width_start_index / 2;

        for (int i = 0; i < h264_encoder->arguments->out_height; i++) {

            for (int j = y_width_start_index; j < h264_encoder->arguments->in_width; j++) {

                int index = h264_encoder->arguments->in_width *
                            (h264_encoder->arguments->out_height - i - 1) + j;
                uint8_t value = *(picture_buf + index);
                *(h264_encoder->pFrame->data[0] +
                  (h264_encoder->arguments->out_width - (j - y_width_start_index) - 1)
                  * h264_encoder->arguments->out_height +
                  i) = value;
            }
        }
        for (int i = 0; i < h264_encoder->arguments->out_height / 2; i++) {
            for (int j = uv_width_start_index; j < h264_encoder->arguments->in_width / 2; j++) {
                int index = h264_encoder->arguments->in_width / 2 *
                            (h264_encoder->arguments->out_height / 2 - i - 1) + j;
                uint8_t v = *(picture_buf + in_y_size + index);
                uint8_t u = *(picture_buf + in_y_size * 5 / 4 + index);
                *(h264_encoder->pFrame->data[2] +
                  (h264_encoder->arguments->out_width / 2 - (j - uv_width_start_index) - 1)
                  * h264_encoder->arguments->out_height / 2 +
                  i) = v;
                *(h264_encoder->pFrame->data[1] +
                  (h264_encoder->arguments->out_width / 2 - (j - uv_width_start_index) - 1)
                  * h264_encoder->arguments->out_height / 2 +
                  i) = u;
            }
        }
    }
}

/**
 * 视频编码结束
 * @return
 */
int H264Encode::encodeEnd() {
    //Flush Encoder
    int ret_1 = flush_encoder(pFormatCtx, 0);
    if (ret_1 < 0) {
        LOG_E(DEBUG, "Flushing encoder failed\n");
        return -1;
    }

    //Write file trailer
    av_write_trailer(pFormatCtx);

    //Clean
    if (video_st) {
        avcodec_close(video_st->codec);
        av_free(pFrame);
//        av_free(picture_buf);
    }
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    LOG_I(DEBUG, "视频编码结束");
//    arguments->handler->setup_video_state(END_STATE);
//    arguments->handler->try_encode_over(arguments);
    return 1;
}

/**
 * 用户中断
 */
void H264Encode::user_end() {
    is_end = END;
}

void H264Encode::release() {
    is_release = RELEASE;
}