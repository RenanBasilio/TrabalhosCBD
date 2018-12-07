//
// Created by Tuniks on 12/4/2018.
//
#include "Equijoin.hpp"


namespace Join {
    MemoryWrapper<DataBlock<Registro>> initializeMem1() {
        int vhd = vhdf::openDisk("testdisk.vhd");
        return MemoryWrapper<DataBlock<Registro>>(vhd);
    }

    MemoryWrapper<DataBlock<RegistroPartido>> initializeMem2() {
        int vhd = vhdf::openDisk("partydisk.vhd");
        return MemoryWrapper<DataBlock<RegistroPartido>>(vhd);
    }

    MemoryWrapper<DataBlock<Registro>> initializeHeap1() {
        remove("VHDHeap.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;

        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    MemoryWrapper<DataBlock<RegistroPartido>> initializeHeap2() {
        remove("VHDPartyHeap.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem = MemoryWrapper<DataBlock<RegistroPartido>>(vhdf::openDisk("VHDPartyHeap.vhd", sizeof(RegistroPartido)*40000, true));
        Schema<RegistroPartido> schema = Schema<RegistroPartido>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;

        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    MemoryWrapper<DataBlock<Registro>> initializeHash1() {
        remove("VHDHash.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDHash.vhd", sizeof(Registro)*40000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 2000;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
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
        MemoryWrapper<DataBlock<RegistroPartido>> mem = MemoryWrapper<DataBlock<RegistroPartido>>(vhdf::openDisk("VHDPartyHash.vhd", sizeof(RegistroPartido)*40000, true));
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

    MemoryWrapper<DataBlock<Registro>> initializeOrdered1() {
        remove("VHDOrdenado.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDOrdenado.vhd", sizeof(Registro)*40000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = ORDERED;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
        schema.chave = 19;   // NR_PARTIDO

        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    MemoryWrapper<DataBlock<RegistroPartido>> initializeOrdered2() {
        remove("VHDPartyOrdenado.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem = MemoryWrapper<DataBlock<RegistroPartido>>(vhdf::openDisk("VHDPartyOrdenado.vhd", sizeof(RegistroPartido)*40000, true));
        Schema<RegistroPartido> schema = Schema<RegistroPartido>();
        schema.org = ORDERED;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
        schema.chave = 0; // NR_PARTIDO

        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void runTestsNested() {
        //Base
        MemoryWrapper<DataBlock<Registro>> mem1 = initializeMem1();
        MemoryWrapper<DataBlock<RegistroPartido>> mem2 = initializeMem2();

        //HEAP
        MemoryWrapper<DataBlock<Registro>> memHeap1 = initializeHeap1();
        MemoryWrapper<DataBlock<RegistroPartido>> memHeap2 = initializeHeap2();

        //ORDERED
        MemoryWrapper<DataBlock<Registro>> memOrdered1 = initializeOrdered1();
        MemoryWrapper<DataBlock<RegistroPartido>> memOrdered2 = initializeOrdered2();

        //HASH
        MemoryWrapper<DataBlock<Registro>> memHash1 = initializeHash1();
        MemoryWrapper<DataBlock<RegistroPartido>> memHash2 = initializeHash2();

        std::vector<Registro> inserts = std::vector<Registro>();
        std::vector<RegistroPartido> insertsPartido = std::vector<RegistroPartido>();
        for (int i = 0; i < 1000; i++) {
            mem1.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                if(mem1->isRegistroEscrito(j)) {
                    inserts.push_back(mem1->getRegistro(j));
                }
            }
        }
        for (int i = 0; i < 1; i++) {
            mem2.loadBlock(1+i);
            for (int j = 0; j < mem2->registrosEscritos.size(); j++) {
                if(mem2->isRegistroEscrito(j)) {
                    insertsPartido.push_back(mem2->getRegistro(j));
                }
            }
        }
        std::cout << insertsPartido.size() << " - " << inserts.size() << std::endl;
        Hash::INSERT(memHash1, inserts);
        Hash::INSERTPARTY(memHash2, insertsPartido);
        Ordered::INSERT<Registro>(memOrdered1, inserts);
        Ordered::INSERT<RegistroPartido>(memOrdered2, insertsPartido);
        Heap::INSERT<Registro>(memHeap1, inserts);
        Heap::INSERT<RegistroPartido>(memHeap2, insertsPartido);

        try {
            //HEAP
            memHeap1.blockAccessCount = 0;
            memHeap2.blockAccessCount = 0;
            std::vector<std::pair<Registro, RegistroPartido>> vect;
            vect = Join::NESTEDJOIN(memHeap1, memHeap2, {"NR_PARTIDO=18"});
            std::cout << vect[0].first.NR_PARTIDO << " - " << vect[0].second.NM_PARTIDO << " - " << vect.size() << std::endl;
            std::cout << memHeap1.blockAccessCount << " - " << memHeap2.blockAccessCount << std::endl;

            //ORDERED
            memOrdered1.blockAccessCount = 0;
            memOrdered2.blockAccessCount = 0;
            std::vector<std::pair<Registro, RegistroPartido>> vect2;
            vect2 = Join::NESTEDJOIN(memOrdered1, memOrdered2, {"NR_PARTIDO=18"});
            std::cout << vect2[0].first.NR_PARTIDO << " - " << vect2[0].second.NM_PARTIDO << " - " << vect2.size() << std::endl;
            std::cout << memOrdered1.blockAccessCount << " - " << memOrdered2.blockAccessCount << std::endl;

            //HASH
            memHash1.blockAccessCount = 0;
            memHash2.blockAccessCount = 0;
            std::vector<std::pair<Registro, RegistroPartido>> vect3;
            vect3 = Join::NESTEDJOIN(memHash1, memHash2, {"NR_PARTIDO=18"});
            std::cout << vect3[0].first.NR_PARTIDO << " - " << vect3[0].second.NM_PARTIDO << " - " << vect3.size() << std::endl;
            std::cout << memHash1.blockAccessCount << " - " << memHash2.blockAccessCount << std::endl;

        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }

//        vhdf::closeDisk(mem1.getDiskId());
//        vhdf::closeDisk(mem2.getDiskId());
//        vhdf::closeDisk(memHash1.getDiskId());
//        vhdf::closeDisk(memHash2.getDiskId());
//        vhdf::closeDisk(memOrdered1.getDiskId());
//        vhdf::closeDisk(memOrdered2.getDiskId());
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

        std::cout << schema1.ultimo_bloco << std::endl;
        //std::cout << "Comparing against database members..." << std::endl;
        for (size_t i = schema1.primeiro_bloco; i <= schema1.ultimo_bloco; i++) {
            mem1.loadBlock(i);
            for (int j = 0; j < mem1->registrosEscritos.size(); j++) {
                if(mem1->isRegistroEscrito(j)) {
                    reg1 = mem1->getRegistro(j);
                    joinTarget.campo = targets2[0].campo;   //has to be targets2 campo
                    joinTarget.valor = {getValorCampo(targets1[0].campo, &reg1)};
                    joinTarget.tipo = targets1[0].tipo;

                    //inner loop
                    for (size_t l = schema2.primeiro_bloco; (l <= schema2.ultimo_bloco); l++) {
                        mem2.loadBlock(l);
                        for (int m = 0; m < mem2->registrosEscritos.size(); m++) {
                            if(mem2->isRegistroEscrito(m)) {
                                reg2 = mem2->getRegistro(m);
                                if ((matchQuery(joinTarget, &reg2))) {
                                    ret_regs.push_back(std::make_pair(reg1, reg2));
                                }
                            }
                        }
                    }
                }
            }
        }
        return ret_regs;
    }
}