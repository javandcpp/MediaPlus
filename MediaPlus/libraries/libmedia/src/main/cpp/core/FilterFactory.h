//
// Created by developer on 11/28/17.
//

#include "MediaFilter.h"
#include "DrawTextFilter.h"

#ifndef MEDIAPLUS_FILTERFACTORY_H
#define MEDIAPLUS_FILTERFACTORY_H

class FilterFactory {
private:
    FilterFactory();

public:
    ~FilterFactory();

//    mutable mutex mut;

    MediaFilter *drawTextFilter= nullptr;

    static FilterFactory *Get();

    DrawTextFilter *createDrawTextFilter(string context,int x,int y);

};

#endif //MEDIAPLUS_FILTERFACTORY_H
