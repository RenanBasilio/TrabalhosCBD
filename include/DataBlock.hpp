#pragma once
#include <bitset>

#include <Registro.hpp>
#include <Util.hpp>


//template <typename T>
class DataBlock {
public:
    std::bitset<Registro::nPorBloco()> registrosEscritos;

    char data[vhdf::BLOCK_SIZE-sizeof(DataBlock::registrosEscritos)-sizeof(size_t)];

    size_t overflow = 0;

    // Recupere um registro de um bloco
    Registro getRegistro(int index);

    // Escreve um registro em um bloco
    void setRegistro(int index, const Registro& reg);

    void deleteRegistro(int index);

    bool isRegistroEscrito(int index);

    int getPrimeiroRegistroDispEscrita();

    static constexpr int tamanhoOffset() {
        return constexpr_ceil(sizeof(DataBlock::registrosEscritos)/8);
    }

    void initialize();

};