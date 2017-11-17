//
// Created by developer on 11/13/17.
//

#ifndef NATIVEAPP_AUDIOENCODEARGS_H
#define NATIVEAPP_AUDIOENCODEARGS_H

#include <iostream>
#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include "../PrefixHeader.h"
using namespace std;
struct AudioEncodeArgs{
    int channels;
    AVSampleFormat avSampleFormat;
    int ch_layout=AV_CH_LAYOUT_STEREO;
    int sampleRate;
    int bitRate;
    int nb_samples;
};

#endif //NATIVEAPP_AUDIOENCODEARGS_H
