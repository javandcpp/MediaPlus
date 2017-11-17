//
// Created by developer on 11/13/17.
//

#ifndef NATIVEAPP_VIDEOENCODEARGS_H
#define NATIVEAPP_VIDEOENCODEARGS_H

#include <iostream>
using namespace std;
struct VideoEncodeArgs{

    int in_width;
    int in_height;
    int out_width;
    int out_height;
    int mirror;
    int fps;

};

#endif //NATIVEAPP_VIDEOENCODEARGS_H
