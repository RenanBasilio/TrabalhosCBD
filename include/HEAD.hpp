#pragma once
#include <Util.hpp>
#include <Data.hpp>
#include <Campo.hpp>
#include <Timestamp.hpp>
#include <DataBlock.hpp>

enum Organizacao { HEAP, ORDERED, HASH };    

template <typename T>
class HEAD
{
private:
    size_t primeiro_bloco, ultimo_bloco;
    // Vetor de tamanhos e localizações relativas dos campos e do registro (usar função initRegistro abaixo)
    // Ponteiros para estruturas secundárias como o identificador de registros deletados
    // Timestamps relevantes à base
    Organizacao org;
public:
    size_t registros_deletados;
    Timestamp criacao, alteracao;
    Campo campos[T::nCampos()];

    HEAD() {
        std::vector<Campo> cps = T::initHEAD(DataBlock::tamanhoOffset());
        memcpy(campos, &cps[0], T::nCampos()*sizeof(Campo));
    };
private:
    char padding[vhdf::BLOCK_SIZE - (sizeof(size_t)*3 + sizeof(Organizacao) + sizeof(Timestamp)*2 + sizeof(Campo)*T::nCampos())];
};


