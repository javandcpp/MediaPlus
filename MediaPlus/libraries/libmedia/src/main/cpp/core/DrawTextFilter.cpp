//
// Created by developer on 11/28/17.
//

#include "DrawTextFilter.h"
DrawTextFilter::~DrawTextFilter() {

}

DrawTextFilter::DrawTextFilter(std::string context, int x, int y) {
    this->mContext.assign(context.c_str());
    this->x=x;
    this->y=y;
}
