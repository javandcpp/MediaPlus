//
// Created by developer on 11/16/17.
//

#include "jni_Video_Process.h"

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LibJniVideoProcess_NV21TOI420(JNIEnv *env,
                                                                              jclass type,
                                                                              jint in_width,
                                                                              jint in_height,
                                                                              jbyteArray srcData_,
                                                                              jbyteArray dstData_) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    VideoProcess::NV21TOI420(in_width, in_height, (const uint8_t *) srcData,
                             (uint8_t *) dstData);

    return 0;
}


JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LibJniVideoProcess_MirrorI420(JNIEnv *env,
                                                                              jclass type,
                                                                              jint in_width,
                                                                              jint in_height,
                                                                              jbyteArray srcData_,
                                                                              jbyteArray dstData_) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    VideoProcess::MirrorI420(in_width, in_height, (const uint8_t *) srcData,
                             (uint8_t *) dstData);

    return 0;
}


JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LibJniVideoProcess_RotateI420(JNIEnv *env,
                                                                              jclass type,
                                                                              jint in_width,
                                                                              jint in_hegith,
                                                                              jbyteArray srcData_,
                                                                              jbyteArray dstData_,
                                                                              jint rotationValue) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    return VideoProcess::RotateI420(in_width, in_hegith, (const uint8_t *) srcData,
                                    (uint8_t *) dstData, rotationValue);
}


#ifdef __cplusplus
}
#endif