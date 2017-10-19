//
// Created by developer on 8/17/17.
//

#ifndef NATIVEAPP_JNI_DEBUG_H
#define NATIVEAPP_JNI_DEBUG_H


#include <android/log.h>


extern int DEBUG;

#define LOG_E(debug, format, ...) if(debug){__android_log_print(ANDROID_LOG_ERROR, "ffmpeg_native", format, ##__VA_ARGS__);}

#define LOG_I(debug, format, ...) if(debug){__android_log_print(ANDROID_LOG_INFO, "ffmpeg_native", format, ##__VA_ARGS__);}

#define LOG_D(debug, format, ...) if(debug){__android_log_print(ANDROID_LOG_DEBUG, "ffmpeg_native", format, ##__VA_ARGS__);}


#endif //NATIVEAPP_JNI_DEBUG_H
