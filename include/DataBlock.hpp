#pragma once
#include <cstring>
#include <Util.hpp>
#include <Schema.hpp>

class DataBlock
{
private:
    friend class MemoryWrapper;
    bool initialized = false;
    size_t recordSize = 0;
public:
    std::vector<bool> registrosEscritos;

    size_t overflow = 0;

    std::vector<void*> registros;

    // Recupere um registro de um bloco
    bool getRegistro(int index, void* reg){
        
        if (!registrosEscritos.at(index)) return false;

        memcpy(reg, registros.at(index), recordSize);

        return true;
    };

    // Escreve um registro em um bloco
    void setRegistro(int index, const void* reg){

        if (!registrosEscritos.at(index)) {
            registrosEscritos.at(index) = true;
        }
        memcpy(registros.at(index), reg, recordSize);

    };

    void deleteRegistro(int index){
        registrosEscritos.at(index) = false;
    };

    bool isRegistroEscrito(int index){
        return registrosEscritos.at(index);
    };

    int getPrimeiroRegistroDispEscrita(){
        for(int i = 0; i < registros.size(); i++)
            if (!registrosEscritos.at(i))
                return i;
        return -1;
    };

    void initialize(size_t size, size_t nRecords){
        recordSize = size;
        registrosEscritos = std::vector<bool>(std::ceil(nRecords/8.0)*8);
        registrosEscritos.assign(registrosEscritos.size(), false);

        if ( !initialized ) {
            registros = std::vector<void*>(nRecords);
            for ( size_t i = 0; i < nRecords; i++ ) {
                registros.at(i) = malloc(recordSize);
            }
            overflow = 0;
        }

        initialized = true;
    };
};