//
// Created by developer on 11/28/17.
//

#include "MediaFilter.h"


#ifndef MEDIAPLUS_DRAWTEXTFILTER_H
#define MEDIAPLUS_DRAWTEXTFILTER_H

class DrawTextFilter : public MediaFilter {


public:
    DrawTextFilter(const char *fontFilePath, const char *context, int *x, int *y);

    ~DrawTextFilter();

    int *x = nullptr;
    int *y = nullptr;
    const char *mContext = nullptr;
    const char *fontFilePath = nullptr;
};

#endif //MEDIAPLUS_DRAWTEXTFILTER_H
