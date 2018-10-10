#include <BlockIO.hpp>

// Recupere um registro de um bloco
Registro getRegistro(int index, void* block) {
    Registro reg;

    block = static_cast<char*>(block) + sizeof(Registro)*index;
    memcpy(&reg, block, sizeof(Registro));

    return reg;
}

// Escreve um registro em um bloco
void setRegistro(int index, void* block, const Registro& reg) {
    block = static_cast<char*>(block) + sizeof(Registro)*index;
    memcpy(block, &reg, sizeof(Registro));
}
