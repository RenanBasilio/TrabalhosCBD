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
    MemoryWrapper<DataBlock> initialize() {
        remove("VHDOrdenado.vhd");
        MemoryWrapper<DataBlock> mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDOrdenado.vhd", sizeof(Registro)*40000, true));
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
        return mem;
    }

    void cleanup(MemoryWrapper<DataBlock> mem) {
        vhdf::closeDisk(mem.getDiskId());
    }

    bool INSERT(MemoryWrapper<DataBlock> mem, std::vector<Registro> registros) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        size_t bloco = schema.ultimo_bloco;

        mem.loadBlock(bloco);
        int regs_processados = mem->getPrimeiroRegistroDispEscrita();
        bool block_changed = false;
        int insertpos;

        for (int i = 0; i<registros.size(); i++) {

            Campo chave = schema.campos[schema.chave];
            //std::cout << chave.nm_campo << std::endl;
            std::string valor = getValorCampo(chave, &registros[i]);

            // Busca binaria
            size_t upper = schema.ultimo_bloco;
            size_t lower = schema.primeiro_bloco;
            size_t middle;

            while (lower < upper) {

                middle = (upper+lower+1)/2;

                mem.loadBlock(middle);

                Registro reg = mem->getRegistro(0);

                if ( comparaCampo(chave, &reg, valor, ">") ) {
                    upper = middle-1;
                } 
                else {
                    lower = middle;
                }
            }
            mem.loadBlock((lower+upper)/2);

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

    // Um select de comparacao simples pode ser feito usando params do tipo {"CAMPO=valor"}
    // Um select de comparacao em uma faixa pode ser feito usando params do tipo {"CAMPO=[min:max]"}
    // Um select de comparacao em um conjunto de valores pode ser feito usando params do tipo {"CAMPO={valor1,valor2}"}
    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> mem, std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs = std::vector<Registro>();

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        Campo chave = schema.campos[schema.chave];

        //std::cout << "Comparing against database members..." << std::endl;
        for (int k = 0; k < targets.size(); k++) {
            switch (targets[k].tipo) {
                case VALUE:
                    std::cout<<schema.ultimo_bloco<<" "<<schema.primeiro_bloco<<std::endl;
                    if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                        size_t upper = schema.ultimo_bloco;
                        size_t lower = schema.primeiro_bloco;
                        size_t middle = (upper+lower)/2;
                        size_t window = upper - lower;

                        std::string valor = targets[k].valor[0];

                        while (window > 1) {

                            mem.loadBlock(middle);

                            Registro reg = mem->getRegistro(0);

                            std::cout << middle << std::endl;
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
                        for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                            reg = mem->getRegistro(j);
                            std::cout << reg.NM_CANDIDATO << std::endl;
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0])){
                                std::cout << "oi" << std::endl;
                                ret_regs.push_back(reg);
                            }
                        }
                    }
                    else{

                    }
                    break;
                case SET:

                    break;
                case RANGE:

                    break;
                default:
                    break;
            }
        }

        return ret_regs;
    }


    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        std::vector<Registro> vect;

//        // Teste com um único insert
//        vhd.loadBlock(1);
//        inserts.push_back(vhd->getRegistro(1));
//
//        INSERT(mem, inserts);

//        // Teste com 5 inserts
//        inserts.clear();
//        vhd.loadBlock(1);
//        for (int i = 0; i < 5; i++) {
//            inserts.push_back(vhd->getRegistro(i));
//        }
//        INSERT(mem, inserts);

        // Teste com 10 inserts
        inserts.clear();
        for (int i = 0; i < 1000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);
        mem.blockAccessCount = 0;

        //Teste Select
        vect = SELECT(mem, {"NM_CANDIDATO=ELLYS DAYANE ALVES MELO"});
        std::cout << mem.blockAccessCount << vect[0].NM_CANDIDATO << std::endl;


        vhdf::closeDisk(vhd.getDiskId());

        cleanup(mem);
    };

}