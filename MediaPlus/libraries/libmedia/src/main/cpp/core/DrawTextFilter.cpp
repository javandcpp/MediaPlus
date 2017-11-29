//
// Created by developer on 11/28/17.
//

#include "DrawTextFilter.h"

DrawTextFilter::~DrawTextFilter() {

}

DrawTextFilter::DrawTextFilter(const char *fontFilePath, const char *context, int *x, int *y)
        : MediaFilter() {
    this->mContext = context;
    this->x = x;
    this->y = y;
    this->fontFilePath = fontFilePath;
    LOG_D(DEBUG, "set draw text filter fontFile:%s context:%s x=%d,y=%d", this->fontFilePath,
          this->mContext, *this->x, *this->y);
}
