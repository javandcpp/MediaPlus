//
// Created by developer on 11/16/17.
//

#ifndef NATIVEAPP_JNI_VIDEO_PROCESS_H
#define NATIVEAPP_JNI_VIDEO_PROCESS_H

#include "../PrefixHeader.h"
#include "../core/VideoProcess.h"


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
using namespace cv;

#ifdef __cplusplus
extern "C" {
#endif




void CopyYUVToImage(uchar *dst, uint8_t *pY, uint8_t *pU, uint8_t *pV, int width, int height) {
    uint32_t size = width * height;
    memcpy(dst, pY, size);
    memcpy(dst + size, pU, size / 4);
    memcpy(dst + size + size / 4, pV, size / 4);
}

void CopyImageToYUV(uint8_t *pY, uint8_t *pU, uint8_t *pV, uchar *src, int width, int height) {
    uint32_t size = width * height;
    memcpy(pY, src, size);
    memcpy(pU, src + size, size / 4);
    memcpy(pV, src + size + size / 4, size / 4);
}

#ifdef __cplusplus
}
#endif


#endif