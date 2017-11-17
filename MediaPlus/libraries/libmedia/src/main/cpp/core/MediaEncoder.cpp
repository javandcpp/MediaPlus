//
// Created by developer on 11/13/17.
//

#include "MediaEncoder.h"

bool MediaEncoder::first = true;

MediaEncoder::MediaEncoder() {
    RegisterAVCodec();
}

MediaEncoder::~MediaEncoder() {

}

/**
 * 注册公用编码器并初始化网络
 */
void MediaEncoder::RegisterAVCodec() {
    //一次初始化
    if (first) {
        first = false;
        av_register_all();
        RegisterAVNetwork();
        LOG_D(DEBUG, "MediaEncoder av_register_all success!");
        LOG_D(DEBUG, "MediaEncoder avformat_network_init success!");
    }
    return;
}

void MediaEncoder::RegisterAVNetwork() {
    avformat_network_init();
    return;
}