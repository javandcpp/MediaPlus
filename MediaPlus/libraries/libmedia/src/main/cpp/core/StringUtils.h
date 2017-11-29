//
// Created by developer on 11/28/17.
//

#ifndef MEDIAPLUS_STRINGUTILS_H
#define MEDIAPLUS_STRINGUTILS_H


#include <sstream>
#include <iostream>

using namespace std;

template<typename T>
class StringUtils {
public:
    std::string to_string(T value) {
        std::ostringstream os;
        os << value;
        return os.str();
    }
};

#endif //MEDIAPLUS_STRINGUTILS_H
