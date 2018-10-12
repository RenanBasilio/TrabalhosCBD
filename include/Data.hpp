#pragma once
#include <Util.hpp>

class Data
{
    public:
        int dia, mes, ano;

        Data() {};
        explicit Data(std::string str, std::string format = "dd/MM/YYYY");
        ~Data() {};
};