//
// Created by developer on 9/7/17.
//


/**
 * JNI网络流转发接口
 */


#include "jni_trans_stream.h"


#ifdef __cplusplus
extern "C" {
#endif


TransStream *transStream = NULL;

JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_stopTrans(JNIEnv *env, jclass type) {
    if (transStream) {
        transStream->isStop = true;
    }
    return 0;
}

JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_transStream(JNIEnv *env, jclass type,
                                                                       jstring inputUri_,
                                                                       jstring outputUri_) {
    const char *inputUri = env->GetStringUTFChars(inputUri_, 0);
    const char *outputUri = env->GetStringUTFChars(outputUri_, 0);
    if (transStream != NULL) {
        delete transStream;
    }
    transStream = new TransStream();
    int ret = 0;
    ret = transStream->Init();

    ret = transStream->openInput(inputUri);
    if (ret < 0) {
        LOG_E(DEBUG, "open input failed");
        goto Error;
    }

    ret = transStream->openOuput(outputUri);
    if (ret < 0) {
        LOG_E(DEBUG, "open output failed");
        goto Error;
    }

    while (true) {
        if (transStream->isStop) {
            transStream->isStop = false;
            break;
        }
        auto packet = transStream->ReadPacketFromSource();
        if (packet) {
            ret = transStream->WritePacket(packet);
            if (ret >= 0) {
                LOG_D(DEBUG, "WritePacket Success!");
            } else {
                LOG_D(DEBUG, "WritePacket failed!");
            }
        }
        else {
            break;
        }
    }

    Error:
    transStream->stopStream();
    return ret;
}

JNIEXPORT jint JNICALL
Java_com_guagua_nativeapp_jnibridge_FFmpegJavaNativeBridge_releaseTransStream(JNIEnv *env,
                                                                              jclass type) {
    // TODO
    if (transStream != nullptr) {
        if (NULL != transStream->avInputFmCxt) {
            avformat_free_context(transStream->avInputFmCxt);
        }
        if (NULL != transStream->avOuputFmCxt) {
            avformat_free_context(transStream->avOuputFmCxt);
        }
        delete transStream;
    }
    return 0;
}

#ifdef __cplusplus
};
#endif
