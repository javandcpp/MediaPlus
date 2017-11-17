//
// Created by developer on 11/12/17.
//

#ifndef NATIVEAPP_ORIGINDATA_H
#define NATIVEAPP_ORIGINDATA_H

#include <iostream>
#include "../PrefixHeader.h"

using namespace std;

class OriginData {
public:
    OriginData();

    ~OriginData();

    int64_t pts=0;
    AVFrame *frame = NULL;
    AVPacket *avPacket = NULL;
    uint8_t *data = NULL;
    int size=NULL;
    void Drop();

};

#endif //NATIVEAPP_ORIGINDATA_H
