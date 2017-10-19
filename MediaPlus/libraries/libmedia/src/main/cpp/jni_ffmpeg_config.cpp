//
// Created by developer on 8/17/17.
//

#include "jni_ffmpeg_config.h"
#include "base_include.h"

jstring getEncoderConfigInfo(JNIEnv *env) {
    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
    return env->NewStringUTF(info);
}