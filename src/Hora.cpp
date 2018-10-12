#include <Hora.hpp>

Hora::Hora(std::string data, std::string format) {
    std::vector<std::string> splits = splitString(data, ':');
    hora = stoi(splits[0]);
    minuto = stoi(splits[1]);
    segundo = stoi(splits[2]);
}