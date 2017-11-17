//
// Created by developer on 11/12/17.
//

#include "OriginData.h"

OriginData::OriginData() {

}

OriginData::~OriginData() {

}

void OriginData::Drop() {
    if (data) {
        free(data);
    }
}
