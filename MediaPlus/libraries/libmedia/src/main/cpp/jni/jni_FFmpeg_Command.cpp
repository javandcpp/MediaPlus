//
// Created by developer on 12/12/17.
//

#include "jni_FFmpeg_Command.h"

#ifdef __cplusplus
extern "C" {
#endif



JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_FFmpegCommandLine_runCommand(JNIEnv *env,
                                                                             jclass type, jint argc,
                                                                             jobjectArray argv) {

    jstring jstr;
    jsize len = env->GetArrayLength(argv);
    char **pstr = (char **) malloc(len*sizeof(char *));

    int i=0;

    for (i=0 ; i<len; i++) {
        jstr = (jstring) env->GetObjectArrayElement(argv, i);
        pstr[i] = (char *)env->GetStringUTFChars(jstr, 0);

        LOG_D(DEBUG,"command:%s",pstr[i]);

    }



    LOG_D(DEBUG,"run command");

    return 0;

}


#ifdef __cplusplus
}
#endif