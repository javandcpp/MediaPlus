//
// Created by developer on 11/13/17.
//

#ifndef NATIVEAPP_VIDEOENCODER_H
#define NATIVEAPP_VIDEOENCODER_H

#include "MediaEncoder.h"
#include "../PrefixHeader.h"
#include "VideoCapture.h"

#include <list>
using namespace std;

class VideoEncoder : public MediaEncoder {

private:
    VideoCapture *videoCapture = NULL;

public:
    AVCodec *avCodec = NULL;
    AVStream *outStream = NULL;
    AVFrame *vOutFrame = NULL;
    AVCodecContext *videoCodecContext = NULL;
    AVFrame *outputYUVFrame = NULL;
    AVPacket videoPacket = {0};

    bool isEncoding = false;

    static VideoEncoder *Get();

    static void *EncodeTask(void *obj);

    /**
     * 已编码数据队列
     */
    threadsafe_queue<OriginData *> vframeQueue;
//    list<OriginData *> VideoDatalist;

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


};


#endif //NATIVEAPP_VIDEOENCODER_H
