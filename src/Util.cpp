#include <Util.hpp>

std::vector<std::string> splitString(std::string str, char delimiter) {
    size_t pos = 0;
    std::vector<std::string> splits;

    while ((pos = str.find(delimiter)) != std::string::npos) {
        splits.push_back(str.substr(0, pos));
        str.erase(0, pos + 1);   
    }

    return splits;
}