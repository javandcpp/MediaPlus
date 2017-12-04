//
// Created by developer on 11/12/17.
//
/**
 * JNI 底层推流
 */
#ifndef NATIVEAPP_JNI_LIVE_MANAGE_H
#define NATIVEAPP_JNI_LIVE_MANAGE_H

#include "../core/VideoCapture.h"
#include "../core/AudioCapture.h"
#include "../PrefixHeader.h"
#include "../core/VideoEncoder.h"
#include "../core/AudioEncoder.h"
#include "../core/RtmpStreamer.h"
#include "../core/AudioCapture.h"
#include "../core/CombineVideoHelper.h"

AudioCapture *audioCapture = NULL;
VideoCapture *videoCapture = NULL;

VideoEncoder *videoEncoder = NULL;
AudioEncoder *audioEncoder = NULL;

RtmpStreamer *rtmpStreamer=NULL;

CombineVideoHelper *combineVideoHelper=NULL;

/**
 * 初始化采集数据接收
 */
bool audioCaptureInit=false;
bool videoCaptureInit=false;
bool isClose = true;
bool isRelease = false;
bool startStream;

mutex mMutex;

#endif
