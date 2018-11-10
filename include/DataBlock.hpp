#pragma once
#include <bitset>
#include <cstring>
#include <Registro.hpp>
#include <Util.hpp>


template <typename T>
class DataBlock {
public:
    std::bitset<T::nPorBloco()> registrosEscritos;

    char data[vhdf::BLOCK_SIZE-sizeof(DataBlock::registrosEscritos)-sizeof(size_t)];

    size_t overflow = 0;

    // Recupere um registro de um bloco
    T getRegistro(int index){
        T reg;

        if (!registrosEscritos.test(index)) return reg;

        void* block = static_cast<char*>(data) + sizeof(T)*index;
        memcpy(&reg, block, sizeof(T));

        return reg;
    };

    // Escreve um registro em um bloco
    void setRegistro(int index, const T& reg){
        void* block = static_cast<char*>(data) + sizeof(T)*index;
        memcpy(block, &reg, sizeof(T));
        registrosEscritos.set(index);
    };

    void deleteRegistro(int index){
        registrosEscritos.reset(index);
    };

    bool isRegistroEscrito(int index){
        return registrosEscritos.test(index);
    };

    int getPrimeiroRegistroDispEscrita(){
        for(int i = 0; i < registrosEscritos.size(); i++)
            if (!registrosEscritos.test(i))
                return i;
        return -1;
    };

    static constexpr int tamanhoOffset() {
        return constexpr_ceil(sizeof(DataBlock::registrosEscritos)/8);
    }

    void initialize(){
        registrosEscritos.reset();
        overflow = 0;
    };

};