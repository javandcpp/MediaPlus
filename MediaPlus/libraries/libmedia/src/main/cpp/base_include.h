//
// Created by developer on 8/17/17.
//


#ifndef NATIVEAPP_BASE_INCLUDE_H
#define NATIVEAPP_BASE_INCLUDE_H

#ifdef __cplusplus
extern "C"{
#endif

#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/opt.h"
#include "include/libavutil/frame.h"
#include "include/libavutil/time.h"
#include "include/libavfilter/avfilter.h"


#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <jni.h>
#include <string.h>
#include <android/log.h>
#include "debug.h"
#include "threadsafe_queue.cpp"
#include "arguments.h"
#include "jni_encode_aac.h"
#include "jni_encode_h264.h"



#define START 0
#define END 1
#define RELEASE 1


#define ROTATE_0_CROP_LT 0

/**
 * 旋转90度剪裁左上
 */
#define ROTATE_90_CROP_LT 1
/**
 * 暂时没处理
 */
#define ROTATE_180 2
/**
 * 旋转270(-90)裁剪左上，左右镜像
 */
#define ROTATE_270_CROP_LT_MIRROR_LR 3


#endif //NATIVEAPP_BASE_INCLUDE_H
