//
// Created by developer on 11/28/17.
//

#include "FilterFactory.h"


FilterFactory::FilterFactory() {

}

FilterFactory::~FilterFactory() {

}

FilterFactory *FilterFactory::Get() {
    static FilterFactory filterFactory;
    return &filterFactory;
}

DrawTextFilter *FilterFactory::createDrawTextFilter(string context, int x, int y) {
    std::lock_guard<std::mutex> lk(mut);
    return shared_ptr<DrawTextFilter>(new DrawTextFilter(context, x, y)).get();
}

