#pragma once
#include <Util.hpp>
#include <Data.hpp>
#include <Hora.hpp>

class Timestamp
{
public:
    Data data;
    Hora hora;

    Timestamp() {};
    Timestamp(const std::string& str) {
        std::vector<std::string> split = splitString(str, ' ');
        hora = Hora(split[0]);
        data = Data(split[1]);
    }
    ~Timestamp() {};
};