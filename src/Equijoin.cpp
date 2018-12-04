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
    MemoryWrapper<DataBlock<T>> initialize_ordered(std::string campo_ordenacao, int num_registros, std::string old_vhd, std::string new_vhd) {
        remove(new_vhd.c_str());
        int vhd_ordenado = vhdf::openDisk(new_vhd.c_str(), sizeof(T)*40000, true);
        MemoryWrapper<DataBlock<T>> mem = MemoryWrapper<DataBlock<T>>(vhd_ordenado);
        Schema<T> schema = Schema<T>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;

        int chave;
        bool found = false;
        for (int i = 0; i < schema.nCampos; i ++){
            if ( schema.campos[i].nm_campo == campo_ordenacao ) {
                chave = i;
                found = true;
            }
        }
        if (found = false) throw std::runtime_error("Campo não encontrado.");

        schema.chave = chave;   /// ordena por esta chave --> 9 = SQ_CANDIDATO; 11 = NM_CANDIDATO

        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        MemoryWrapper<DataBlock<T>> vhd(vhdf::openDisk(old_vhd.c_str()));

        int registrosInseridos = 0;
        // Inserir registros de forma ordenada
        for (int i = 1; i <=std::ceil(float(num_registros)/10); i++) {
            std::vector<T> inserts = std::vector<T>();
            vhd.loadBlock(i);
            for (int j = 0; j < 10; j++) {
                if(registrosInseridos < num_registros) {
                    inserts.push_back(vhd->getRegistro(j));
                    registrosInseridos++;
                }
            }
            Ordered::INSERT(mem, inserts);
        }
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
            std::vector<std::pair<Registro, RegistroPartido>> sortmergejoin_results = SORTMERGEJOIN("NR_PARTIDO",13);
            std::cout << "SortMergeJoin: " << std::endl;
            for(int i = 0; i < sortmergejoin_results.size(); i++) {
                std::cout << sortmergejoin_results[i].first.NR_PARTIDO << ", " << sortmergejoin_results[i].second.NR_PARTIDO << std::endl;
            }
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
        mem.loadBlock(block);

        if(reg_pos < mem->registrosEscritos.count()-1) 
            nextReg = mem->getRegistro(reg_pos+1);
        else if (reg_pos == mem->registrosEscritos.count()-1 && block != schema.ultimo_bloco) {
            mem.loadBlock(block+1);
            nextReg = mem->getRegistro(0);
        } else 
            nextReg.NR_PARTIDO = -1;
        return nextReg;
    }

    std::vector<std::pair<Registro, RegistroPartido>> SORTMERGEJOIN(std::string join_attribute, int num_registros) {
        
        // Inicializar relações
        MemoryWrapper<DataBlock<Registro>> mem_ordered1 = initialize_ordered<Registro>(join_attribute, num_registros, "testdisk.vhd", "vhd_equijoin_registro.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem_ordered2 = initialize_ordered<RegistroPartido>(join_attribute, num_registros, "partydisk.vhd", "vhd_equijoin_registropartido.vhd");

        Schema<Registro> schema1;
        Schema<RegistroPartido> schema2;
        vhdf::readBlock(mem_ordered1.getDiskId(), 0, &schema1);
        vhdf::readBlock(mem_ordered2.getDiskId(), 0, &schema2);

        std::vector<std::pair<Registro, RegistroPartido>> ret_regs = std::vector<std::pair<Registro, RegistroPartido>>();

        Registro reg1;
        Registro reg1_extra;
        RegistroPartido reg2;
        RegistroPartido reg2_extra;
        
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "RELAÇÃO 1" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;

        for (int i=1; i<=(float(num_registros)/schema1.regs_por_bloco)+1; i++) {
            mem_ordered1.loadBlock(i);
            for (int j=0; j<schema1.regs_por_bloco; j++) {
                reg1 = mem_ordered1->getRegistro(j);
                if (reg1.NR_PARTIDO != 0)
                    std::cout << "Registro " << j << ": " << reg1.NR_PARTIDO << std::endl;
            }
        }

        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "RELAÇÃO 2" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;


        for (int i=1; i<=(float(num_registros)/schema2.regs_por_bloco)+1; i++) {
            mem_ordered2.loadBlock(i);
            for (int j=0; j<schema2.regs_por_bloco; j++) {
                reg2 = mem_ordered2->getRegistro(j);
                if (reg2.NR_PARTIDO != 0)
                    std::cout << "Registro " << j << ": " << reg2.NR_PARTIDO << std::endl;
            }
        }
        std::cout << "----------------------------------------------------" << std::endl;


        //Inicializar registros
        mem_ordered1.loadBlock(1);
        mem_ordered2.loadBlock(1);
        reg1 = mem_ordered1->getRegistro(0);
        reg2 = mem_ordered2->getRegistro(0);
        int reg1Count = 0;
        int reg2Count = 0;

        std::cout << "Initializing SortMergeJoin..." << std::endl;

        while(reg1.NR_PARTIDO != -1 && reg2.NR_PARTIDO !=-1) {
            if(reg1.NR_PARTIDO > reg2.NR_PARTIDO) {
                std::cout << "reg1 > reg2" << std::endl;
                reg2 = nextRegistro(reg2Count%schema2.regs_por_bloco,(float(reg2Count)/schema2.regs_por_bloco)+1,mem_ordered2);
                reg2Count++;
            } else if (reg1.NR_PARTIDO < reg2.NR_PARTIDO) {
                std::cout << "reg1 < reg2" << std::endl;
                reg1 = nextRegistro(reg1Count%schema1.regs_por_bloco,(float(reg1Count)/schema1.regs_por_bloco)+1,mem_ordered1);
                reg1Count++;
            } else { //match
                std::cout << "reg1 = reg2 (Match!)" << std::endl;
                ret_regs.push_back(std::make_pair(reg1,reg2));
                // Output further tuples that match with reg1
                int reg2Count_extra = reg2Count;
                reg2_extra = nextRegistro(reg2Count_extra%schema2.regs_por_bloco,(float(reg2Count_extra)/schema2.regs_por_bloco)+1,mem_ordered2);
                while(reg2_extra.NR_PARTIDO != -1 && reg1.NR_PARTIDO == reg2_extra.NR_PARTIDO) {
                    std::cout << "reg1 = reg2' (Match!)" << std::endl;
                    ret_regs.push_back(std::make_pair(reg1,reg2_extra));
                    reg2Count_extra++;
                    reg2_extra = nextRegistro(reg2Count_extra%schema2.regs_por_bloco,(float(reg2Count_extra)/schema2.regs_por_bloco)+1,mem_ordered2);
                }
                // Output further tuples that match with reg2
                int reg1Count_extra = reg1Count;
                reg1_extra = nextRegistro(reg1Count_extra%schema1.regs_por_bloco,(float(reg1Count_extra)/schema1.regs_por_bloco)+1,mem_ordered1);
                while(reg1_extra.NR_PARTIDO != -1 && reg2.NR_PARTIDO == reg1_extra.NR_PARTIDO) {
                    std::cout << "reg1' = reg2 (Match!)" << std::endl;
                    ret_regs.push_back(std::make_pair(reg1_extra,reg2));
                    reg1Count_extra++;
                    reg1_extra = nextRegistro(reg1Count_extra%schema1.regs_por_bloco,(float(reg1Count_extra)/schema1.regs_por_bloco)+1,mem_ordered1);
                }
                reg1 = nextRegistro(reg1Count%schema1.regs_por_bloco,(float(reg1Count)/schema1.regs_por_bloco)+1,mem_ordered1);
                reg2 = nextRegistro(reg2Count%schema2.regs_por_bloco,(float(reg2Count)/schema2.regs_por_bloco)+1,mem_ordered2);
                reg1Count++;
                reg2Count++;
            }
        }     
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