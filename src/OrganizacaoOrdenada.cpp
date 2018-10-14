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
#include <algorithm>

namespace OrganizacaoOrdenada
{
    MemoryWrapper<DataBlock> mem;

    void initialize() {
        remove("VHDOrdenado.vhd");
        mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDOrdenado.vhd", sizeof(Registro)*40000, true));
        HEAD<Registro> schema = HEAD<Registro>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
        //schema.chave = 9;   // SQ_CANDIDATO
        schema.chave = 11; // NM_CANDIDATO
        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
    }

    void cleanup() {
        vhdf::closeDisk(mem.getDiskId());
    }

    bool INSERT(std::vector<Registro> registros) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        size_t bloco = schema.ultimo_bloco;

        mem.loadBlock(bloco);
        int regs_processados = mem->getPrimeiroRegistroDispEscrita();
        bool block_changed = false;
        int insertpos;

        for (int i = 0; i<registros.size(); i++) {

            Campo chave = schema.campos[schema.chave];

            std::string valor = getValorCampo(chave, &registros[i]);

            // Busca binaria
            size_t upper = schema.ultimo_bloco;
            size_t lower = schema.primeiro_bloco;
            size_t middle = (upper+lower)/2;
            size_t window = upper - lower;

            while (window > 1) {

                mem.loadBlock(middle);

                Registro reg = mem->getRegistro(0);

                if ( comparaCampo(chave, &reg, valor, "<=") ) {
                    upper = middle;
                    middle = (upper+lower)/2;
                    window = upper-lower;
                } 
                else {
                    lower = middle;
                    middle = (upper+lower)/2;
                    window = upper-lower;
                }
            }
            mem.loadBlock(middle);

            // Operação de Insert
            Registro held;
            bool insertOK = false;
            bool needsReorder = false;

            for (int j = 0; j < schema.regs_por_bloco; j++) {
                
                Registro reg = mem->getRegistro(j);
                
                if ( !mem->isRegistroEscrito(j) || comparaCampo(chave, &reg, valor, ">") ) {
                    insertOK = true;
                    insertpos = j;
                    if (mem->isRegistroEscrito(j)) {
                        held = mem->getRegistro(j);
                        needsReorder = true;
                    }
                    mem->setRegistro(j, registros[i]);
                    break;
                }
            }
            if (!insertOK) {
                mem.loadBlock(middle+1);
                if (mem.getLoadedBlockId() > schema.ultimo_bloco) mem->initialize();
                if (mem->isRegistroEscrito(0)) {
                    held = mem->getRegistro(0);
                    needsReorder = true;
                }
                mem->setRegistro(0, registros[i]);
                insertpos = 0;
            }

            // Reordenacao
            if (needsReorder) {
                insertpos++;
                Registro placing;
                for (int j = mem.getLoadedBlockId(); j <= schema.ultimo_bloco+1; j++) {
                    bool stop = false;
                    for (insertpos; insertpos < schema.regs_por_bloco; insertpos++) {
                        std::swap(held, placing);
                        
                        if (mem->isRegistroEscrito(insertpos)) held = mem->getRegistro(insertpos);
                        else stop = true;
                        
                        mem->setRegistro(insertpos, placing);
                        if(stop) break;
                    }
                    mem.commitBlock();
                    if (stop) break;
                    mem.loadBlock(j+1);
                    insertpos = 0;
                    if (mem.getLoadedBlockId() > schema.ultimo_bloco) {
                        mem->initialize();
                        schema.ultimo_bloco = mem.getLoadedBlockId();
                    }
                    bloco++;
                }
            }
            else mem.commitBlock();
        }
        
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        return true;
    }


    void runTests() {
        initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        // Teste com um único insert
        vhd.loadBlock(1);
        inserts.push_back(vhd->getRegistro(1));
        INSERT(inserts);

        // Teste com 5 inserts
        inserts.clear();
        vhd.loadBlock(2);
        for (int i = 0; i < 5; i++) {
            inserts.push_back(vhd->getRegistro(i));
        }
        INSERT(inserts);

        // Teste com 10 inserts
        inserts.clear();
        for (int i = 0; i < 1; i++) {
            vhd.loadBlock(3+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(inserts);

        vhdf::closeDisk(vhd.getDiskId());

        cleanup();
    };

}