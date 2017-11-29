//
// Created by developer on 11/28/17.
//

#include "MediaFilter.h"


#ifndef MEDIAPLUS_DRAWTEXTFILTER_H
#define MEDIAPLUS_DRAWTEXTFILTER_H

class DrawTextFilter :public MediaFilter{


public:
    DrawTextFilter(std::string context,int x,int y);
    ~DrawTextFilter();
    int x=0;
    int y=0;
    string mContext="";
};

#endif //MEDIAPLUS_DRAWTEXTFILTER_H
