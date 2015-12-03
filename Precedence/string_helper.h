#pragma once
#include <string>

class StringHelper {
public:
    static bool EndsWith(std::string &str, std::string &ending) {
        if (str.length() < ending.length())
            return false;
        return 0 == str.compare(str.length() - ending.length(), ending.length(), ending);
    }
};