//
// Created by Tuniks on 12/4/2018.
//
#include "Equijoin.hpp"


namespace Join {
    MemoryWrapper<DataBlock<Registro>> initializeHeap1() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("testdisk.vhd");
        return MemoryWrapper<DataBlock<Registro>>(vhd);
    }

    MemoryWrapper<DataBlock<RegistroPartido>> initializeHeap2() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("partydisk.vhd");
        return MemoryWrapper<DataBlock<RegistroPartido>>(vhd);
    }

    MemoryWrapper<DataBlock<Registro>> initializeHash1() {
        remove("VHDHash.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDHash.vhd", sizeof(Registro)*40000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 2000;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
        //schema.chave = 9;   // SQ_CANDIDATO
        //schema.chave = 11; // NM_CANDIDATO
        schema.chave = 19;   // NR_PARTIDO

        for (int i = 1; i < schema.blocos_hash+1; i++) {
            mem.loadBlock(i);
            mem->initialize();
            mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    MemoryWrapper<DataBlock<RegistroPartido>> initializeHash2() {
        remove("VHDPartyHash.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem = MemoryWrapper<DataBlock<RegistroPartido>>(vhdf::openDisk("VHDPartyHash.vhd", sizeof(Registro)*40000, true));
        Schema<RegistroPartido> schema = Schema<RegistroPartido>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 2000;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
        schema.chave = 0; // NR_PARTIDO

        for (int i = 1; i < schema.blocos_hash+1; i++) {
            mem.loadBlock(i);
            mem->initialize();
            mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void runTestsNested() {
        //HEAP
        MemoryWrapper<DataBlock<Registro>> mem1 = initializeHeap1();
        MemoryWrapper<DataBlock<RegistroPartido>> mem2 = initializeHeap2();

        //ORDERED
        //HASH

        try {
            //HEAP
            std::vector<std::pair<Registro, RegistroPartido>> vect;
            vect = Join::NESTEDJOIN(mem1, mem2, {"NR_PARTIDO=18"});
            std::cout << vect[0].first.NR_PARTIDO << " - " << vect[0].second.NM_PARTIDO << " - " << vect.size() << std::endl;

            //ORDERED
            //HASH

        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }

        vhdf::closeDisk(mem1.getDiskId());
        vhdf::closeDisk(mem2.getDiskId());
    }

    std::vector<std::pair<Registro, RegistroPartido>>
    NESTEDJOIN(MemoryWrapper<DataBlock<Registro>> &mem1, MemoryWrapper<DataBlock<RegistroPartido>> &mem2,
               std::vector<std::string> params) {
        Schema<Registro> schema1;
        vhdf::readBlock(mem1.getDiskId(), 0, &schema1);
        Schema<RegistroPartido> schema2;
        vhdf::readBlock(mem2.getDiskId(), 0, &schema2);

        std::vector<std::pair<Registro, RegistroPartido>> ret_regs = std::vector<std::pair<Registro, RegistroPartido>>();

        std::vector<Target> targets1 = parseQuery(schema1, params);
        std::vector<Target> targets2 = parseQuery(schema2, params);
        Target joinTarget;

        Registro reg1;
        RegistroPartido reg2;

        bool found = false;

        //std::cout << "Comparing against database members..." << std::endl;
        for (size_t i = schema1.primeiro_bloco; i <= schema1.ultimo_bloco; i++) {
            mem1.loadBlock(i);
            for (int j = 0; j < mem1->registrosEscritos.size(); j++) {
                reg1 = mem1->getRegistro(j);

                joinTarget.campo = targets2[0].campo;   //has to be targets2 campo
                joinTarget.valor = {getValorCampo(targets1[0].campo,&reg1)};
                joinTarget.tipo = targets1[0].tipo;

                //inner loop
                found = false;
                for (size_t l = schema2.primeiro_bloco; (l <= schema2.ultimo_bloco) && (!found); l++) {
                    mem2.loadBlock(l);
                    for (int m = 0; m < mem2->registrosEscritos.size(); m++) {
                        reg2 = mem2->getRegistro(m);

                        if((matchQuery(joinTarget, &reg2)) && (!found)){
                            found = true;
                            ret_regs.push_back(std::make_pair(reg1, reg2));
                        }
                    }
                }
            }
        }
        return ret_regs;
    }
}