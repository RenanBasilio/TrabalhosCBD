#include <DataBlock.hpp>
#include <cstring>

// Recupere um registro de um bloco
Registro DataBlock::getRegistro(int index) {
    Registro reg;
    
    if (!registrosEscritos.test(index)) return reg;

    void* block = static_cast<char*>(data) + sizeof(Registro)*index;
    memcpy(&reg, block, sizeof(Registro));

    return reg;
};

// Escreve um registro em um bloco
void DataBlock::setRegistro(int index, const Registro& reg) {

    void* block = static_cast<char*>(data) + sizeof(Registro)*index;
    memcpy(block, &reg, sizeof(Registro));
    registrosEscritos.set(index);
};

int DataBlock::getPrimeiroRegistroDispEscrita() {
    for(int i = 0; i < registrosEscritos.size(); i++)
        if (!registrosEscritos.test(i))
            return i;
}

void DataBlock::deleteRegistro(int index) {
    registrosEscritos.reset(index);
}