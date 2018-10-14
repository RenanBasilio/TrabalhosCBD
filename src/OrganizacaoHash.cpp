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
#include <OrganizacaoHash.hpp>

namespace OrganizacaoHash
{
    MemoryWrapper<DataBlock> initialize() {
        remove("VHDOrdenado.vhd");
        MemoryWrapper<DataBlock> mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHash.vhd", sizeof(Registro)*40000, true));
        HEAD<Registro> schema = HEAD<Registro>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 27;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
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

        for (int i = 0; i<registros.size(); i++) {

            Campo chave = schema.campos[schema.chave];
            //std::cout << chave.nm_campo << std::endl;
            std::string valor = getValorCampo(chave, &registros[i]);

            // Funcao hash
            size_t bloco;
            switch (chave.tipo) {
                case INT:
                    bloco = hashingFunc(schema.blocos_hash, std::stoi(valor));
                    break;
                case BIGINT:
                    bloco = hashingFunc(schema.blocos_hash, std::atoll(valor.c_str()));
                    break;
                case CHAR:
                    bloco = hashingFunc(schema.blocos_hash, valor);
                    break;
                case BOOL:
                    break;
            }

            mem.loadBlock(bloco+schema.primeiro_bloco);

            // Operação de Insert
            bool insertOK = false;

            int insertpos = mem->getPrimeiroRegistroDispEscrita();
            while (insertpos == -1) {
                if (mem->overflow == 0) {
                    mem.loadBlock(schema.ultimo_bloco+1);
                    mem->initialize();
                    insertpos = 0;
                } else {
                    mem.loadBlock(mem->overflow);
                    insertpos = mem->getPrimeiroRegistroDispEscrita();
                }
            }
            mem->setRegistro(insertpos, registros[i]);
            mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        return true;
    }

    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        /*
        // Teste com um único insert
        vhd.loadBlock(1);
        inserts.push_back(vhd->getRegistro(1));

        INSERT(mem, inserts);

        // Teste com 5 inserts
        inserts.clear();
        vhd.loadBlock(1);
        for (int i = 0; i < 5; i++) {
            inserts.push_back(vhd->getRegistro(i));
        }
        INSERT(mem, inserts);
        */

        // Teste com 10 inserts
        inserts.clear();
        for (int i = 0; i < 1000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);

        cleanup(mem);
    };

}