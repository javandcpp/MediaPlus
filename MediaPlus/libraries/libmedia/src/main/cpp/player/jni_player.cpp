//
// Created by developer on 12/25/17.
//



#include <pthread.h>
#include "jni_player.h"
#include "../core/debug.h"
#include "../ffmpeg/sdl2/SDL.h"
#include "../ffmpeg/sdl2/SDL_thread.h"
#include "../base_include.h"


#ifdef __cplusplus
extern "C" {
#endif

pthread_mutex_t mutex;





static jint _SetVideoSurface(JNIEnv *env,
                            jobject instance,
                            jobject surface) {

    pthread_mutex_lock(&mutex);
    LOG_D(DEBUG, "setViewSurface")


//    ANativeWindow_fromSurface();
    pthread_mutex_unlock(&mutex);
    return 0;
}
//
//JNIEXPORT void JNICALL
//Java_app_mobile_nativeapp_com_mediaplus_player_MediaPlusPlayer__1setDataSource(JNIEnv *env,
//                                                                               jobject instance,
//                                                                               jstring path_) {
//    const char *path = env->GetStringUTFChars(path_, 0);
//
//
//    env->ReleaseStringUTFChars(path_, path);
//}
//
//JNIEXPORT void JNICALL
//Java_app_mobile_nativeapp_com_mediaplus_player_MediaPlusPlayer__1initPlayer(JNIEnv *env,
//                                                                            jobject instance) {
//    //注册FFmpeg所有编解码及复用器
////    JNI_CreateJavaVM();
//
//
//    av_register_all();
//    //注册网络组件
//    avformat_network_init();
//
//
//}
static JNINativeMethod g_methods[] = {
        {"setVideoSurface","(Landroid/view/Surface;)V",(void*)_SetVideoSurface}

};

void JNI_OnLoad_Play(JavaVM *vm, void *reserved) {

    JNIEnv* env = NULL;
    javaVM = vm;
    if ((vm)->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    assert(env != NULL);
    jclass clazz = env->FindClass("app/mobile/nativeapp/com/libmedia/core/player/MediaPlusPlayer");  //获取Java Native
    (env)->RegisterNatives(clazz, g_methods, NELEM(g_methods));
    LOG_D(DEBUG, "my jni onload");
}

#ifdef __cplusplus
}
#endif


