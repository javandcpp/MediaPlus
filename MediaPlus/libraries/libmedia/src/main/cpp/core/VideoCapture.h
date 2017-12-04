//
// Created by developer on 11/12/17.
//

#ifndef NATIVEAPP_VIDEOCAPTURE_H
#define NATIVEAPP_VIDEOCAPTURE_H

#include <list>
#include "MediaCapture.h"
#include "../PrefixHeader.h"
using namespace std;


/**
 * 视频数据接收
 */
class VideoCapture : public MediaCapture {

private:
    VideoCapture();

    VideoEncodeArgs *videoEncodeArgs = NULL;
public:
    ~VideoCapture();

    static VideoCapture *Get();

    bool CloseCapture();

    bool StartCapture();

    int Release();

    int PushVideoData(OriginData *originData);

    void SetCameraID(CameraID cameraID);

    CameraID GetCameraID();

    OriginData *GetVideoData();


    CameraID mCameraId;


    /**
   * 设置编码参数
   */
    void SetVideoEncodeArgs(VideoEncodeArgs *videoEncodeArgs);

    VideoEncodeArgs *GetVideoEncodeArgs();


//    list<OriginData *> VideoCaptureDatalist;

    threadsafe_queue<OriginData *> videoCaputureframeQueue;
    /**
     * 获取视频采集状态
     */
    bool GetCaptureState();

    /**
   * nv21源数据处理(旋转)
   */
    uint8_t *NV21ProcessYUV420P(int in_width, int in_height, int out_width, int out_heigth,
                                uint8_t *src, uint8_t *dst, CameraID cameraID, int needMirror);

    bool enableWaterMark;
};

#endif //NATIVEAPP_VIDEOCAPTURE_H
