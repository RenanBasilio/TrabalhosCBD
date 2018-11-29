#include "Equijoin.hpp"

namespace Join {
    MemoryWrapper<DataBlock<Registro>> initialize1() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("testdisk.vhd");
        return MemoryWrapper<DataBlock<Registro>>(vhd);
    }

    MemoryWrapper<DataBlock<RegistroPartido>> initialize2() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("partydisk.vhd");
        return MemoryWrapper<DataBlock<RegistroPartido>>(vhd);
    }

    MemoryWrapper<DataBlock<Registro>> initialize_ordered(int chave, int num_registros) {
        remove("VHDOrdenado_equijoin.vhd");
        int vhd_ordenado = vhdf::openDisk("VHDOrdenado_equijoin.vhd", sizeof(Registro)*40000, true);
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhd_ordenado);
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
        schema.chave = chave;   /// ordena por esta chave
        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);


        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock<Registro>> vhd(vhdf::openDisk("testdisk.vhd"));

        // Inserir registros de forma ordenada
        inserts.clear();
        for (int i = 0; i < num_registros/10; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        Ordered::INSERT(mem, inserts);

        return mem;
    }



    void runTests() {
        MemoryWrapper<DataBlock<Registro>> mem1 = initialize1();
        MemoryWrapper<DataBlock<RegistroPartido>> mem2 = initialize2();
        MemoryWrapper<DataBlock<Registro>> mem_ordered = initialize_ordered(11, 10);


        try {
            std::vector<std::pair<Registro, RegistroPartido>> vect;
            std::cout << "oi" << std::endl;
            vect = Join::NESTEDJOIN(mem1, mem2, {"NR_PARTIDO=18"});
            std::cout << vect[0].first.NM_CANDIDATO << " - " << vect.size() << std::endl;
        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }
        vhdf::closeDisk(mem1.getDiskId());
        vhdf::closeDisk(mem2.getDiskId());
        vhdf::closeDisk(mem_ordered.getDiskId());
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

        Registro reg1;
        RegistroPartido reg2;

        bool found = false;

        //std::cout << "Comparing against database members..." << std::endl;
        for (size_t i = schema1.primeiro_bloco; i <= schema1.ultimo_bloco; i++) {
            mem1.loadBlock(i);
            for (int j = 0; j < mem1->registrosEscritos.size(); j++) {
                reg1 = mem1->getRegistro(j);

                int match = 0;
                for (int k = 0; k < targets1.size(); k++) matchQuery(targets1[k], &reg1) ? match++ : match;
                if (match == targets1.size()) {
                    //inner loop
                    found = false;
                    for (size_t l = schema2.primeiro_bloco; l <= schema2.ultimo_bloco; l++) {
                        mem2.loadBlock(l);
                        for (int m = 0; m < mem2->registrosEscritos.size(); m++) {
                            reg2 = mem2->getRegistro(m);
                            //std::cout << "l00p" << std::endl;
                            int match = 0;
                            for (int n = 0; n < targets2.size(); n++) matchQuery(targets2[n], &reg2) ? match++ : match;
                            if ((match == targets2.size()) && (!found)) {
                                //std::cout << "oi" << std::endl;
                                found = true;
                                ret_regs.push_back(std::make_pair(reg1, reg2));
                            }
                        }
                    }
                    //ret_regs.push_back(reg1);
                    //std::cout << "Found match " << reg.NM_CANDIDATO << std::endl;
                }
            }
        }
        return ret_regs;
    }
}