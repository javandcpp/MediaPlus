////
//// Created by developer on 12/4/17.
////
//
//#ifndef MEDIAPLUS_COMBINEVIDEOHELPER_H
//#define MEDIAPLUS_COMBINEVIDEOHELPER_H
//
//
//
//extern "C" {
//#include <string.h>
//#include <stdio.h>
//#include "../TypeDefs.h"
//#include "../GlobalDefs.h"
//}
//
//struct WaterMarkConfig {
//
//    ~WaterMarkConfig() {
//        if (waterByteData) {
//            free(waterByteData);
//        }
//    }
//
//    unsigned char enable;
//
//    int frameWidth = 0;
//    int frameHeight = 0;
//    int x = 0;
//    int y = 0;
//    int waterWidth = 0;
//    int waterHeight = 0;
//    BYTE *waterByteData = nullptr;
//};
//
//class CombineVideoHelper {
//
//
//private:
//    WaterMarkConfig *waterMarkConfig = nullptr;
//    CombineVideoHelper();
//
//public:
//    ~CombineVideoHelper();
//
//    static CombineVideoHelper *Instance();
//
//
//    int CombineWaterMark(uint8_t *dst, uint8_t *&src);
//
//    void SetWaterMarkConfig(WaterMarkConfig *&waterMarkConfig);
//
//};
//
//#endif //MEDIAPLUS_COMBINEVIDEOHELPER_H
