#pragma once

#include <Campo.hpp>
#include <DataBlock.hpp>
#include <MemoryWrapper.hpp>
#include <Registro.hpp>
#include <Schema.hpp>
#include <Util.hpp>
#include <Query.hpp>

namespace Heap
{
    void runTests();

    //bool INSERT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<Registro> registros);

    std::vector<Registro> SELECT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params);

    bool DELETE(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params);

    template <typename T>
    bool INSERT(MemoryWrapper<DataBlock<T>> &mem, std::vector<T> registros) {

    Schema<T> schema;
    vhdf::readBlock(mem.getDiskId(), 0, &schema);

    size_t pos_inicial = schema.ultimo_bloco;

    mem.loadBlock(pos_inicial);
    int blocos_processados = 0;
    int regs_processados = mem->getPrimeiroRegistroDispEscrita();
    bool block_changed = false;

    for (int i = 0; i<registros.size(); i++) {
        regs_processados = mem->getPrimeiroRegistroDispEscrita();
        while (regs_processados == -1) {
            if (block_changed) mem.commitBlock();
            blocos_processados++;
            mem.loadBlock(pos_inicial+blocos_processados);
            if (mem.getLoadedBlockId() > schema.ultimo_bloco) mem->initialize();
            block_changed = false;
            regs_processados = mem->getPrimeiroRegistroDispEscrita();
        }
        mem->setRegistro(regs_processados, registros[i]);
        block_changed = true;
    }
    mem.commitBlock();

    if (schema.ultimo_bloco != pos_inicial+blocos_processados) {
        schema.ultimo_bloco = pos_inicial+blocos_processados;
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
    }

    return true;
    }
}