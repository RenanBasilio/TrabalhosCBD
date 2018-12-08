#pragma once
#include <fstream>
#include <string>
#include <vhdf.hpp>
#include <cmath>
#include <cstring>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <Schema.hpp>
#include <Parser/Registro.hpp>

template <typename T>
void parseStream(MemoryWrapper &mem, std::ifstream& ifs, int numRegistros) {
    std::string str;
    Schema schema = mem.getSchema();

    size_t lastblock = schema.ultimo_bloco;
    size_t regsprocessed = 0;
    size_t totalregsprocessed = 0;

    mem.loadBlock(lastblock);

    while(getline(ifs, str) && totalregsprocessed < numRegistros) {
        // Se o numero de registros por bloco foi atingido, comitta no vhd
        if (regsprocessed == schema.regs_por_bloco) {
            mem.commitBlock();
            regsprocessed = 0;
            lastblock++;
            mem.loadBlock(lastblock);
        }
        T reg(str);
        mem->setRegistro(regsprocessed, &reg);
        regsprocessed++;
        totalregsprocessed++;
    }
    mem.commitBlock();
    lastblock++;

    schema.ultimo_bloco = lastblock;
    mem.updateSchema(schema);
}



