//
// Created by developer on 11/12/17.
//
/**
 * JNI 底层推流
 */
#include <jni.h>
#include "Jni_Live_Manage.h"
#include "../core/FilterFactory.h"




#ifdef __cplusplus
extern "C" {
#endif

/**
 * 关闭底层采集、编码、推流
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_Close(JNIEnv *env,
                                                                          jclass type) {
    mMutex.lock();

    //关闭采集
    if (NULL != audioCapture) {
        audioCapture->CloseCapture();
    }
    if (NULL != videoCapture) {
        videoCapture->CloseCapture();
    }
    //关闭推流
    if (NULL != rtmpStreamer) {
        rtmpStreamer->ClosePushStream();
    }
    //关闭编码
    if (NULL != videoEncoder) {
        videoEncoder->CloseEncode();
    }
    if (NULL != audioEncoder) {
        audioEncoder->CloseEncode();
    }


    isClose = true;
    startStream = false;
    LOG_D(DEBUG, "jni close");
    mMutex.unlock();
    return 0;
}

/**
 * 底层资源回收
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_Release(JNIEnv *env,
                                                                            jclass type) {
    mMutex.lock();
    isRelease = true;
    mMutex.unlock();
    return 0;
}

/**
 * 接收APP层数据,AAC编码
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_EncodeAAC(JNIEnv *env,
                                                                              jclass type,
                                                                              jbyteArray audioBuffer_,
                                                                              jint length) {
    if (audioCaptureInit && !isClose) {
        jbyte *audioSrc = env->GetByteArrayElements(audioBuffer_, 0);
        uint8_t *audioDstData = (uint8_t *) malloc(length);
        memcpy(audioDstData, audioSrc, length);
        OriginData *audioOriginData = new OriginData();
        audioOriginData->size = length;
        audioOriginData->data = audioDstData;
        audioCapture->PushAudioData(audioOriginData);
        env->ReleaseByteArrayElements(audioBuffer_, audioSrc, 0);
    }
    return 0;
}
/**
 * 接收APP层数据,H264编码
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_EncodeH264(JNIEnv *env,
                                                                               jclass type,
                                                                               jbyteArray videoBuffer_,
                                                                               jint length) {
    if (videoCaptureInit && !isClose) {
        jbyte *videoSrc = env->GetByteArrayElements(videoBuffer_, 0);
        uint8_t *videoDstData = (uint8_t *) malloc(length);
        memcpy(videoDstData, videoSrc, length);
        OriginData *videoOriginData = new OriginData();
        videoOriginData->size = length;
        videoOriginData->data = videoDstData;
        videoCapture->PushVideoData(videoOriginData);
        env->ReleaseByteArrayElements(videoBuffer_, videoSrc, 0);
    }
    return 0;
}


/**
 * 初始化音频编码器
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_InitAudioEncoder(JNIEnv *env,
                                                                                     jclass type) {
    mMutex.lock();
    if (audioCaptureInit) {
        audioEncoder = AudioEncoder::Get();
        audioEncoder->SetAudioCapture(audioCapture);
        mMutex.unlock();
        return audioEncoder->InitEncode();

    }
    LOG_D(DEBUG, "jni InitAudioEncoder failed!");
    mMutex.unlock();
    return -1;
}
/**
 * 初始化视频编码器
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_InitVideoEncoder(JNIEnv *env,
                                                                                     jclass type) {
    mMutex.lock();
    if (videoCaptureInit) {
        videoEncoder = VideoEncoder::Get();
        videoEncoder->SetVideoCapture(videoCapture);
        mMutex.unlock();
        return videoEncoder->InitEncode();
    }
    LOG_D(DEBUG, "jni InitVideoEncoder failed!");
    mMutex.unlock();
    return -1;
}

/**
 * 初始化采集
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_InitAudioCapture(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jint channles,
                                                                                     jint SampleRate,
                                                                                     jint SampleBitRate) {
    mMutex.lock();
    audioCapture = AudioCapture::Get();
    AudioEncodeArgs *audioEncodeArgs = (AudioEncodeArgs *) malloc(sizeof(AudioEncodeArgs));
    audioEncodeArgs->avSampleFormat = AV_SAMPLE_FMT_S16;
    audioEncodeArgs->sampleRate = SampleRate;
    audioEncodeArgs->bitRate = SampleBitRate;
    audioEncodeArgs->channels = channles;
    if (audioEncodeArgs->channels == 1) {
        audioEncodeArgs->ch_layout = AV_CH_LAYOUT_MONO;
    } else if (audioEncodeArgs->channels == 2) {
        audioEncodeArgs->ch_layout = AV_CH_LAYOUT_STEREO;
    }
    audioEncodeArgs->nb_samples = 1024;
    audioCapture->SetAudioEncodeArgs(audioEncodeArgs);
    audioCaptureInit = true;
    isRelease = false;
    mMutex.unlock();
    return 0;
}

/**
 * 初始化采集
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_InitVideoCapture(JNIEnv *env,
                                                                                     jclass type,
                                                                                     jint inWidth,
                                                                                     jint inHeight,
                                                                                     jint outWidth,
                                                                                     jint outHeight,
                                                                                     jint fps,
                                                                                     jboolean mirror) {
    mMutex.lock();
    videoCapture = VideoCapture::Get();
    VideoEncodeArgs *videoEncodeArgs = (VideoEncodeArgs *) malloc(sizeof(VideoEncodeArgs));
    videoEncodeArgs->fps = fps;
    videoEncodeArgs->in_height = inHeight;
    videoEncodeArgs->in_width = inWidth;
    videoEncodeArgs->out_height = outHeight;
    videoEncodeArgs->out_width = outWidth;
    videoEncodeArgs->mirror = mirror;
    videoCapture->SetVideoEncodeArgs(videoEncodeArgs);
    videoCaptureInit = true;
    isRelease = false;
    mMutex.unlock();
    return 0;
}


/**
 * 开始推流
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_StartPush(JNIEnv *env,
                                                                              jclass type,
                                                                              jstring url_) {
    mMutex.lock();
    if (videoCaptureInit && audioCaptureInit) {
        startStream = true;
        isClose = false;
        videoCapture->StartCapture();
        audioCapture->StartCapture();
        const char *url = env->GetStringUTFChars(url_, 0);
//        LOG_D(DEBUG, "stream URL:%s", url);
//        if (NULL != videoEncoder) {
//            videoEncoder->StartEncode();
//        }
//        if (NULL != audioEncoder) {
//            audioEncoder->StartEncode();
//        }
        rtmpStreamer = RtmpStreamer::Get();
        //初始化推流器
        if (rtmpStreamer->InitStreamer(url) != 0) {
            LOG_D(DEBUG, "jni initStreamer success!");
            mMutex.unlock();
            return -1;
        }
        rtmpStreamer->SetVideoEncoder(videoEncoder);
        rtmpStreamer->SetAudioEncoder(audioEncoder);
        if (rtmpStreamer->StartPushStream() != 0) {
            LOG_D(DEBUG, "jni push stream failed!");
            videoCapture->CloseCapture();
            audioCapture->CloseCapture();
            rtmpStreamer->ClosePushStream();
            mMutex.unlock();
            return -1;
        }
        LOG_D(DEBUG, "jni push stream success!");
        env->ReleaseStringUTFChars(url_, url);
    }
    mMutex.unlock();
    return 0;
}


/**
 * 添加视频滤镜
 */
JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_Filter(JNIEnv *env, jclass type,
                                                                           jobject filterType,
                                                                           jint filterValue) {

    // TODO
    return 0;
}

JNIEXPORT void JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_SetCameraID(JNIEnv *env,
                                                                                jclass type,
                                                                                jint cameraID) {
    if (videoCaptureInit && !isClose && !isRelease) {
        if (cameraID == CameraID::FRONT) {
            videoCapture->SetCameraID(CameraID::FRONT);
        } else if (cameraID == CameraID::BACK) {
            videoCapture->SetCameraID(CameraID::BACK);
        }
    }
    return;
}

JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_DrawText(JNIEnv *env,
                                                                             jclass type,
                                                                             jstring fontPath_,
                                                                             jstring text_,
                                                                             jint x, jint y) {
    const char *text = env->GetStringUTFChars(text_, 0);
    const char *fontPath = env->GetStringUTFChars(fontPath_, 0);
    int ret = 0;
    if (NULL != videoEncoder) {
        FilterFactory *filterFactory = FilterFactory::Get();
        DrawTextFilter *pTextFilter = filterFactory->createDrawTextFilter(fontPath,text, &x, &y);
        ret = videoEncoder->SetFilter(pTextFilter);
    }
    env->ReleaseStringUTFChars(text_, text);
    return ret;
}

JNIEXPORT jint JNICALL
Java_app_mobile_nativeapp_com_libmedia_core_jni_LiveJniMediaManager_SetWaterMark(JNIEnv *env,
                                                                                 jobject instance,
                                                                                 jboolean enable,
                                                                                 jbyteArray waterMark,
                                                                                 jint waterWidth,
                                                                                 jint waterHeight,
                                                                                 jint positionX,
                                                                                 jint positionY) {
    jbyte *waterMark_ = env->GetByteArrayElements(waterMark, NULL);
    if (videoCaptureInit) {
        WaterMarkConfig *waterMarkConfig = new WaterMarkConfig();
        videoCapture->enableWaterMark = enable;
        waterMarkConfig->frameWidth = videoCapture->GetVideoEncodeArgs()->in_height;//480
        waterMarkConfig->frameHeight = videoCapture->GetVideoEncodeArgs()->in_width;//640
        waterMarkConfig->waterWidth = waterWidth;
        waterMarkConfig->waterHeight = waterHeight;
        waterMarkConfig->x = positionX;
        waterMarkConfig->y = positionY;
        uint8_t *waterData = (uint8_t *) malloc(waterWidth * waterHeight * 4);
        memcpy(waterData, waterMark_, waterWidth * waterHeight * 4);
        waterMarkConfig->waterByteData = waterData;
        combineVideoHelper = CombineVideoHelper::Instance();
        combineVideoHelper->SetWaterMarkConfig(waterMarkConfig);

    }
    env->ReleaseByteArrayElements(waterMark, waterMark_, 0);
    return 0;
}


#ifdef __cplusplus
}
#endif
