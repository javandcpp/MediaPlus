//
// Created by developer on 12/12/17.
//

#include <jni.h>
#include "debug.h"
#include "FFmpegCommandLine.h"


int jniFFmpegCommandRegister(JavaVM *vm) {


    return 0;
}


JNIEXPORT jint JNICALL RunCommand(JNIEnv *env,
                                  jobject instance,
                                  jint argc,
                                  jobjectArray argv) {

    LOG_D(DEBUG, "RunCommand");

    return 0;
}

