#pragma once
#include <Util.hpp>

class Hora
{
public:
    int hora, minuto, segundo;
    Hora() {};
    explicit Hora(std::string str, std::string format = "HH:MM:SS");
    ~Hora() {};
};