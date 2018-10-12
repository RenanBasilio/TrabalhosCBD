#include <Data.hpp>

Data::Data(std::string data, std::string format) {
    std::vector<std::string> splits = splitString(data, '/');
    dia = stoi(splits[0]);
    mes = stoi(splits[1]);
    ano = stoi(splits[2]);
}