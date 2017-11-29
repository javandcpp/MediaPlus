//
// Created by developer on 11/13/17.
//

#ifndef NATIVEAPP_VIDEOENCODER_H
#define NATIVEAPP_VIDEOENCODER_H

#include "MediaEncoder.h"
#include "../PrefixHeader.h"
#include "VideoCapture.h"
#include "DrawTextFilter.h"

#include <string>
#include <iostream>

#include <list>

using namespace std;

class VideoEncoder : public MediaEncoder {

private:
    VideoCapture *videoCapture = NULL;

    /**
  *
  */
    int InitFilter();

public:


    bool isEncoding = false;

    static VideoEncoder *Get();

    static void *EncodeTask(void *obj);

    /**
     * 已编码数据队列
     */
    threadsafe_queue<OriginData *> vframeQueue;
//    list<OriginData *> VideoDatalist;

    DrawTextFilter **drawTextFilter = nullptr;

    VideoEncoder();

    ~VideoEncoder();

    /**
     * 开启编码
     */
    int StartEncode();

    /**
     * 初始化H264视频编码器
     */
    int InitEncode();

    /**
     * 关闭编码器
     */
    int CloseEncode();


    int EncodeH264(OriginData **originData);

    /**
     * 资源回收
     */
    int Release();

    /**
     * 设置数据采集
     */
    void SetVideoCapture(VideoCapture *videoCapture);


    VideoCapture *GetVideoCapture();


    /**
     * 获取编码器状态
     */
    bool GetEncodeState();


    /**
     * 前置镜像
     */
    void YUVProcessMirror();

    /**
     * 初始化过滤器
     */
    int SetFilter(DrawTextFilter *drawTextFilter);

    AVCodec *avCodec = NULL;

    AVStream *outStream = NULL;

    AVFrame *vOutFrame = NULL;

    AVCodecContext *videoCodecContext = NULL;

    AVFrame *outputYUVFrame = NULL;

    AVFrame *inputYUVFrame = NULL;

    AVPacket videoPacket = {0};

    AVFilter *buffersrc = nullptr;

    AVFilter *buffersink = nullptr;

    AVFilterInOut *outputs= nullptr;
    AVFilterInOut *inputs= nullptr;
    AVFilterGraph *filter_graph= nullptr;

    AVFilterContext *buffersink_ctx  = nullptr;;
    AVFilterContext *buffersrc_ctx  = nullptr;;



};


#endif //NATIVEAPP_VIDEOENCODER_H
