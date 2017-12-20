//
// Created by developer on 12/12/17.
//

#include "jni_FFmpeg_Command.h"

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_FFmpegCommandLine_runCommand(JNIEnv *env,
                                                                             jclass type, jint len,
                                                                             jobjectArray objectArray) {

    int argc = env->GetArrayLength(objectArray);
    char *argv[argc];
    jstring jstr[argc];

    int i = 0;;
    for (i = 0; i < argc; i++) {
        jstr[i] = (jstring) env->GetObjectArrayElement(objectArray, i);
        argv[i] = (char *) env->GetStringUTFChars(jstr[i], 0);
        LOG_D(DEBUG," command[%d]:%s",i,argv[i]);
    }
    int status = Run(argc, argv);
    for (i = 0; i < argc; ++i) {
        env->ReleaseStringUTFChars(jstr[i], argv[i]);
    }
    return status;

}


#ifdef __cplusplus
}
#endif