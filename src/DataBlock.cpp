#include <DataBlock.hpp>
#include <cstring>

// Recupere um registro de um bloco
Registro DataBlock::getRegistro(int index) {
    Registro reg;

    void* block = static_cast<char*>(data) + sizeof(Registro)*index;
    memcpy(&reg, block, sizeof(Registro));

    return reg;
};

// Escreve um registro em um bloco
void DataBlock::setRegistro(int index, const Registro& reg) {

    void* block = static_cast<char*>(data) + sizeof(Registro)*index;
    memcpy(block, &reg, sizeof(Registro));
};
