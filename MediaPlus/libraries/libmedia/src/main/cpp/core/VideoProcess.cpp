//
// Created by developer on 11/16/17.
//

#include "VideoProcess.h"

int VideoProcess::MirrorI420(int in_width, int in_height,
                             const uint8_t *srcData, uint8_t *dstData) {
    return I420Mirror(srcData, in_width,
                      srcData + (in_width * in_height), in_width / 2,
                      srcData + (in_width * in_height * 5 / 4), in_width / 2,
                      dstData, in_width,
                      dstData + (in_width * in_height), in_width / 2,
                      dstData + (in_width * in_height * 5 / 4), in_width / 2,
                      in_width, in_height);
}

int VideoProcess::NV21TOI420(int in_width, int in_height,
                             const uint8_t *srcNV21Data, uint8_t *dstI420Data) {

    return NV21ToI420((const uint8_t *) srcNV21Data, in_width,
                      (uint8_t *) srcNV21Data + (in_width * in_height), in_width,
                      dstI420Data, in_width,
                      dstI420Data + (in_width * in_height), in_width / 2,
                      dstI420Data + (in_width * in_height * 5 / 4), in_width / 2,
                      in_width, in_height);
}

int VideoProcess::RotateI420(int in_width, int in_height,
                             const uint8_t *srcData, uint8_t *dstData,
                             int rotationValue) {

    RotationMode rotationMode = kRotate0;
    switch (rotationValue) {
        case 90:
            rotationMode = kRotate90;
            break;
        case 180:
            rotationMode = kRotate180;
            break;
        case 270:
            rotationMode = kRotate270;
            break;
    }
    return I420Rotate(srcData, in_width,
                      srcData + (in_width * in_height), in_width / 2,
                      srcData + (in_width * in_height * 5 / 4), in_width / 2,
                      dstData, in_height,
                      dstData + (in_width * in_height), in_height / 2,
                      dstData + (in_width * in_height * 5 / 4), in_height / 2,
                      in_width, in_height,
                      rotationMode);
}