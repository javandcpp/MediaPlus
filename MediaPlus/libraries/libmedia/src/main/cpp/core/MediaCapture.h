//
// Created by developer on 11/12/17.
//

#ifndef NATIVEAPP_MEDIACAPTURE_H
#define NATIVEAPP_MEDIACAPTURE_H

#include "threadsafe_queue.cpp"
#include "OriginData.h"
#include "debug.h"
#include "../PrefixHeader.h"

class MediaCapture {

protected:

    mutable mutex mut;

    MediaCapture();

    virtual ~MediaCapture();

    virtual bool CloseCapture() = 0;

    virtual int Release() = 0;

    bool ExitCapture;
public:

};

#endif //NATIVEAPP_MEDIACAPTURE_H
