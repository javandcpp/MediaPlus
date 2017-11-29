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

    mutable mutex mut;

    static FilterFactory *Get();

    DrawTextFilter *createDrawTextFilter(const char *fontPath, const char *context, int *x, int *y);

};

#endif //MEDIAPLUS_FILTERFACTORY_H
