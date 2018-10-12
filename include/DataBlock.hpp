#pragma once
#include <bitset>

#include <Registro.hpp>

class DataBlock {
public:
    std::bitset<Registro::nPorBloco()> registrosEscritos;

    char data[vhdf::BLOCK_SIZE-sizeof(DataBlock::registrosEscritos)];

    // Recupere um registro de um bloco
    Registro getRegistro(int index);

    // Escreve um registro em um bloco
    void setRegistro(int index, const Registro& reg);
};