//
// Created by developer on 11/16/17.
//

#ifndef NATIVEAPP_VIDEOPROCESS_H
#define NATIVEAPP_VIDEOPROCESS_H


#import "../PrefixHeader.h"
using namespace libyuv;

/**
 * YUV色彩空间处理
 */

class VideoProcess {
public:

    /**
     * android camera data NV21 convert to I420
     */
    static int NV21TOI420(int in_width, int in_hegith,
                          const uint8_t *srcData,
                          uint8_t *dstData);

    /**
     * mirror I420
     */
    static int MirrorI420(int in_width, int in_hegith,
                          const uint8_t *srcData,
                          uint8_t *dstData);

    /**
     * rotate I420 by RotationMode
     */
    static int RotateI420(int in_width, int in_hegith,
                          const uint8_t *srcData,
                          uint8_t *dstData, int rotationMode);

};

#endif //NATIVEAPP_LIBVIDEOPROCESS_H
