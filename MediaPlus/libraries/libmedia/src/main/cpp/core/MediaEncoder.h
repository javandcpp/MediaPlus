//
// Created by developer on 11/13/17.
//

#ifndef NATIVEAPP_MEDIAENCODER_H
#define NATIVEAPP_MEDIAENCODER_H

#include "../PrefixHeader.h"
#include "debug.h"

class MediaEncoder {
protected:

    mutable mutex mut;

    MediaEncoder();

    virtual ~MediaEncoder();

    void RegisterAVCodec();

    void RegisterAVNetwork();

    virtual int StartEncode() = 0;

    virtual int InitEncode() = 0;

    virtual int CloseEncode() = 0;

    virtual int Release() = 0;

    static bool first;


};


#endif //NATIVEAPP_MEDIAENCODER_H
