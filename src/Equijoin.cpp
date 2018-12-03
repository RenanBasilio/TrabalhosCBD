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

    template <typename T>
    MemoryWrapper<DataBlock<T>> initialize_ordered(int chave, int num_registros, std::string old_vhd, std::string new_vhd) {
        remove(new_vhd.c_str());
        int vhd_ordenado = vhdf::openDisk(new_vhd.c_str(), sizeof(T)*40000, true);
        MemoryWrapper<DataBlock<T>> mem = MemoryWrapper<DataBlock<T>>(vhd_ordenado);
        Schema<T> schema = Schema<T>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
        schema.chave = chave;   /// ordena por esta chave --> 9 = SQ_CANDIDATO; 11 = NM_CANDIDATO
        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        MemoryWrapper<DataBlock<T>> vhd(vhdf::openDisk(old_vhd.c_str()));

/*         Schema<T> schema_new1;
        vhdf::readBlock(mem.getDiskId(), 0, &schema_new1);
        std::cout << "Ultimo bloco mem1: " << schema_new1.ultimo_bloco << std::endl; */

        int registrosInseridos = 0;
        // Inserir registros de forma ordenada
        for (int i = 1; i <=std::ceil(float(num_registros)/10); i++) {
            std::vector<T> inserts = std::vector<T>();
            vhd.loadBlock(i);
            for (int j = 0; j < 10; j++) {
                if(registrosInseridos < num_registros) {
                    //std::cout << "Inseridos: " << registrosInseridos << std::endl;
                    inserts.push_back(vhd->getRegistro(j));
                    registrosInseridos++;
                }
            }
            Ordered::INSERT(mem, inserts);
        }

        //vhdf::closeDisk(mem.getDiskId());
/* 
        Schema<T> schema_new2;
        vhdf::readBlock(mem.getDiskId(), 0, &schema_new2);
        std::cout << "Ultimo bloco mem2: " << schema_new2.ultimo_bloco << std::endl; */
        return mem;
    }



    void runTests() {
        MemoryWrapper<DataBlock<Registro>> mem1 = initialize1();
        MemoryWrapper<DataBlock<RegistroPartido>> mem2 = initialize2();
        //MemoryWrapper<DataBlock<Registro>> mem_ordered = initialize_ordered<Registro>(11, 13, );


        try {
            //std::vector<std::pair<Registro, RegistroPartido>> vect;
            //vect = Join::NESTEDJOIN(mem1, mem2, {"NR_PARTIDO=18"});
            //std::cout << vect[0].first.NM_CANDIDATO << " - " << vect.size() << std::endl;
            Join::SORTMERGEJOIN(11,13);
        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }
        vhdf::closeDisk(mem1.getDiskId());
        vhdf::closeDisk(mem2.getDiskId());
        //vhdf::closeDisk(mem_ordered.getDiskId());
    }

    template <typename T>
    T nextRegistro(int reg_pos, int block, MemoryWrapper<DataBlock<T>> &mem) {

        T nextReg;
        Schema<T> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

/*         std::cout << "Ultimo bloco: " << schema.ultimo_bloco << std::endl;
        std::cout << "Bloco atual: " << block << std::endl;
 */

        mem.loadBlock(block);
        if(reg_pos < mem->registrosEscritos.count()-1) {
            nextReg = mem->getRegistro(reg_pos+1);
        } else if (reg_pos == mem->registrosEscritos.count()-1 && block != schema.ultimo_bloco) {
            mem.loadBlock(block+1);
            nextReg = mem->getRegistro(0);
        } else {
/*             std::cout << "Deu merda" << std::endl; */
        }
/*         if(schema.ultimo_bloco != block) {
            if (reg_pos == 9) {
                mem.loadBlock(block+1);
                nextReg = mem->getRegistro(0);
            }
            else {
                mem.loadBlock(block);
                nextReg = mem->getRegistro(reg_pos+1);
            }
        }
        else {
            std::cout << "Ultimo bloco" << std::endl;
            mem.loadBlock(block);
            if(reg_pos < mem->registrosEscritos.count()-1)
                nextReg = mem->getRegistro(reg_pos+1);
        } */
        return nextReg;
    }

    std::vector<std::pair<Registro, RegistroPartido>> SORTMERGEJOIN(int join_attribute, int num_registros) {
        
        // Inicializar relações
        MemoryWrapper<DataBlock<Registro>> mem_ordered1 = initialize_ordered<Registro>(join_attribute, num_registros, "testdisk.vhd", "vhd_equijoin_registro.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem_ordered2 = initialize_ordered<RegistroPartido>(join_attribute, num_registros, "partydisk.vhd", "vhd_equijoin_registropartido.vhd");

        Schema<Registro> schema1;
        Schema<RegistroPartido> schema2;
        vhdf::readBlock(mem_ordered1.getDiskId(), 0, &schema1);
        vhdf::readBlock(mem_ordered2.getDiskId(), 0, &schema2);

        std::vector<std::pair<Registro, RegistroPartido>> ret_regs = std::vector<std::pair<Registro, RegistroPartido>>();

        // std::vector<Target> targets1 = parseQuery(schema1, params);
        // std::vector<Target> targets2 = parseQuery(schema2, params);

        Registro reg1;
        RegistroPartido reg2;
        bool found = false;

        //mem_ordered1.loadBlock(1);
        Registro reg_extra = nextRegistro(-1,1,mem_ordered1);
        std::cout << "Primeiro registro: " << reg_extra.NM_CANDIDATO << std::endl;
        

        for (int i=1; i<=std::ceil(float(num_registros)/schema2.regs_por_bloco); i++) {
            // mem_ordered1.loadBlock(i+1);
            for (int j=0; j<schema2.regs_por_bloco; j++) {
                reg2 = nextRegistro(j,i,mem_ordered2);
                std::cout << "Registro " << j+1 << ": " << reg2.NM_PARTIDO << std::endl;
            }
        }

        //Inicializar registros
/*         mem_ordered1.loadBlock(1);
        mem_ordered2.loadBlock(1);
        reg1 = mem_ordered1->getRegistro(0);
        reg2 = mem_ordered2->getRegistro(0);
        int i = 0;

        while(reg1) {
            
        }

        for (size_t i = schema1.primeiro_bloco; i <= schema1.ultimo_bloco; i++) {
            mem_ordered1.loadBlock(i);
        }
        */
        return ret_regs;
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