//
// Created by developer on 11/12/17.
//

#ifndef NATIVEAPP_PREFIXHEADER_H
#define NATIVEAPP_PREFIXHEADER_H

#include <iostream>
#include <mutex>
#include <memory>
#include <condition_variable>
#include "core/VideoEncodeArgs.h"
#include "base_include.h"
#include "core/AudioEncodeArgs.h"


using namespace std;
//测试数据写入文件
#if 0
#define WRITE_YUV_TO_FILE
#define WRITE_PCM_TO_FILE
#endif


#if 0
#define ENCODE_INFO
#endif

#if 0
#define PTS_INFO
#endif

#if 0
#define SHOW_DEBUG_INFO
#endif



enum CameraID {
    FRONT, BACK
};


#endif //NATIVEAPP_PREFIXHEADER_H
