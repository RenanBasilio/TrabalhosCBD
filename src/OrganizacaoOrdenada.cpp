#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <HEAD.hpp>
#include <Registro.hpp>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <vhdf.hpp>
#include <Campo.hpp>
#include <exception>
#include <Query.hpp>

namespace OrganizacaoOrdenada
{
    MemoryWrapper<DataBlock> mem;

    void initialize() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("testdisk.vhd");
        mem = MemoryWrapper<DataBlock>(vhd);
    }

    void cleanup() {
        vhdf::closeDisk(mem.getDiskId());
    }

    bool INSERT(std::vector<Registro> registros) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        size_t pos_inicial = schema.ultimo_bloco;

        mem.loadBlock(pos_inicial);
        int blocos_processados = 0;
        int regs_processados = mem->getPrimeiroRegistroDispEscrita();
        bool block_changed = false;

        for (int i = 0; i<registros.size(); i++) {

            size_t first = schema.ultimo_bloco;
            size_t last = schema.primeiro_bloco;

            if (regs_processados == schema.regs_por_bloco) {
                if (block_changed) mem.commitBlock();
                blocos_processados++;
                mem.loadBlock(pos_inicial+blocos_processados);
                block_changed = false;
                regs_processados = mem->getPrimeiroRegistroDispEscrita();
            }
            if (!mem->isRegistroEscrito(i)) {
                mem->setRegistro(regs_processados, registros[i]);
                block_changed = true;
            }
            else {
                regs_processados = mem->getPrimeiroRegistroDispEscrita();
                while (regs_processados != 1) {
                    if (block_changed) mem.commitBlock();
                    blocos_processados++;
                    mem.loadBlock(pos_inicial+blocos_processados);
                    block_changed = false;
                    regs_processados = mem->getPrimeiroRegistroDispEscrita();
                }
            }

        }
        mem.commitBlock();

        schema.ultimo_bloco = pos_inicial+blocos_processados;
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        return true;
    }


    void runTests() {
        initialize();
        cleanup();
    };

}