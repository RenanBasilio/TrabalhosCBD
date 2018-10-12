#pragma once
#include <Util.hpp>

class Timestamp: public Data, public Hora
{
    Timestamp() {};
    Timestamp(std::str datastr, std::str horastr): Data(datastr), Hora(horastr){}
    ~Timestamp() {};
};