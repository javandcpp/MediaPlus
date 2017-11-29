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


JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LibJniVideoProcess_Beauty(JNIEnv *env, jclass type,
                                                                          jint in_width,
                                                                          jint in_height,
                                                                          jbyteArray srcData_,
                                                                          jbyteArray dstData_) {
    jbyte *srcData = env->GetByteArrayElements(srcData_, NULL);
    jbyte *dstData = env->GetByteArrayElements(dstData_, NULL);

    timeval start;
    timeval end;

    gettimeofday(&start, NULL);
    LOG_D(DEBUG, "")
//
//    cv::Mat *mainYUuvImage=new Mat();
//    cv::Mat *dstYUVImage=new Mat();
//
//    mainYUuvImage.create(in_height * 3 / 2, in_width, CV_8UC1);
//    CopyYUVToImage(mainYUuvImage.data, (uint8_t *) srcData,
//                   (uint8_t *) srcData + (in_width * in_height),
//                   (uint8_t *) srcData + (in_width * in_height * 5 / 4), in_width, in_height);
//    cv::Mat mainRgbImage
//    cvtColor 颜色空间转换
//    cv::cvtColor(mainYUuvImage, mainRgbImage, CV_YUV2BGR_I420);
    //双边滤波
//    cv::bilateralFilter(mainYUuvImage, dstYUVImage, 5, 5*2, 5/2, BORDER_CONSTANT);
//
//    CopyImageToYUV((uint8_t *) dstData, (uint8_t *) dstData + (in_width * in_height),
//                   (uint8_t *) dstData + (in_width * in_height * 5 / 4), dstYUVImage.data, in_width,
//                   in_height);
//
//    gettimeofday(&end, NULL);
//    LOG_D(DEBUG, "opencv bilateralFilter hosttime millseconds:%d", (end.tv_usec - start.tv_usec) / 1000);
    env->ReleaseByteArrayElements(srcData_, srcData, 0);
    env->ReleaseByteArrayElements(dstData_, dstData, 0);

//    cv::fastFree(dstYUVImage);
//    cv::fastFree(mainYUuvImage);
//    delete mainYUuvImage;
//    delete dstYUVImage;

    return 0;
}


#ifdef __cplusplus
}
#endif