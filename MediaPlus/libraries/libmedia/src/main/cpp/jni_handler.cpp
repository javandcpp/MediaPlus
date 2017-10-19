
/**
 * Created by jianxi on 2017/5/26.
 * https://github.com/mabeijianxi
 * mabeijianxi@gmail.com
 */
#include "jni_handler.h"
#include "base_include.h"

/**
 * 改变视频录制状态
 * @param video_state
 */
void JXJNIHandler::setup_video_state(int video_state) {
    JXJNIHandler::video_state = video_state;
}
/**
 * 改变音频录制状态
 * @param audio_state
 */
void JXJNIHandler::setup_audio_state(int audio_state) {
    JXJNIHandler::audio_state = audio_state;
}

/**
 * 检查是否视音是否都完成，如果完成就开始合成
 * @param arguments
 * @return
 */
int JXJNIHandler::try_encode_over(UserArguments *arguments) {
//    if (audio_state == END_STATE && video_state == END_STATE) {
//        start_muxer(arguments);
//        return END_STATE;
//    }
    return 0;
}

/**
 * 开始视频合成
 * @param arguments
 * @return
 */
int JXJNIHandler::start_muxer(UserArguments *arguments) {
//    JXMediaMuxer *muxer = new JXMediaMuxer();
//    muxer->startMuxer(arguments->video_path, arguments->audio_path, arguments->media_path);
//    delete (muxer);
//    end_notify(arguments);
    return 0;
}

/**
 * 通知java层
 * @param arguments
 */
void JXJNIHandler::end_notify(UserArguments *arguments) {
//    try {
        int status;

        JNIEnv *env;
        status = arguments->javaVM->AttachCurrentThread(&env, NULL);
        if (status < 0) {
            LOG_E(DEBUG,"callback_handler: failed to attach "
                         "current thread");
            return;
        }

        jmethodID pID = env->GetStaticMethodID(arguments->java_class, "notifyState", "(IF)V");

        if (pID == NULL) {
            LOG_E(DEBUG,"callback_handler: failed to get method ID");
            arguments->javaVM->DetachCurrentThread();
            return;
        }

        env->CallStaticVoidMethod(arguments->java_class, pID, END, 0);
        env->DeleteGlobalRef(arguments->java_class);
        LOG_I(DEBUG,"啦啦啦---succeed");
        arguments->javaVM->DetachCurrentThread();

//    }
//    catch (exception e) {
//        LOGI(JNI_DEBUG,"反射回调失败");
//    }

    delete (arguments);
    delete(this);
}