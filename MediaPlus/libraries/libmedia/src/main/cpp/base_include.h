//
// Created by developer on 8/17/17.
//


#ifndef NATIVEAPP_BASE_INCLUDE_H
#define NATIVEAPP_BASE_INCLUDE_H



#include <jni.h>
#include <string>
#include "core/debug.h"
#include <android/log.h>
#include "core/threadsafe_queue.cpp"

#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include "ffmpeg/libavcodec/avcodec.h"
#include "ffmpeg/libavformat/avformat.h"
#include "ffmpeg/libavutil/opt.h"
#include "ffmpeg/libavutil/avutil.h"
#include "ffmpeg/libavutil/frame.h"
#include "ffmpeg/libavutil/time.h"
#include "ffmpeg/libavfilter/avfilter.h"
#include "ffmpeg/libswscale/swscale.h"
#include "ffmpeg/libavutil/pixfmt.h"
#include "ffmpeg/libavutil/imgutils.h"
#include "yuv/libyuv.h"
#include "ffmpeg/libswresample/swresample.h"

#ifdef __cplusplus
}
#endif



#endif //NATIVEAPP_BASE_INCLUDE_H
