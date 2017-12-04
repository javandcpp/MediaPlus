//
// Created by developer on 12/4/17.
//

#ifndef MEDIAPLUS_COMBINEVIDEOHELPER_H
#define MEDIAPLUS_COMBINEVIDEOHELPER_H

#include "../PrefixHeader.h"

#define RGBRound(a) a>255?255:a<0?0:a


struct WaterMarkConfig {

    ~WaterMarkConfig() {
        if (waterByteData) {
            free(waterByteData);
        }
    }

    unsigned char enable;

    int frameWidth = 0;
    int frameHeight = 0;
    int x = 0;
    int y = 0;
    int waterWidth = 0;
    int waterHeight = 0;
    uint8_t *waterByteData = nullptr;
};

class CombineVideoHelper {


private:
    WaterMarkConfig *waterMarkConfig = nullptr;

    CombineVideoHelper();

public:
    ~CombineVideoHelper();

    static CombineVideoHelper *Instance();


    int CombineWaterMark(uint8_t *dst, uint8_t *&src);

    void SetWaterMarkConfig(WaterMarkConfig *&waterMarkConfig);

};

#endif //MEDIAPLUS_COMBINEVIDEOHELPER_H
