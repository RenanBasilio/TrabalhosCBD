#include "Equijoin.hpp"

namespace Join {

    // Verifica se o resultado encontrado já está vetor de resultados
    bool isResultDuplicate (std::pair<Registro, RegistroPartido> result, std::vector<std::pair<Registro, RegistroPartido>> result_vec) {

        for(int i=0; i<result_vec.size(); i++) {
            if(result.first.SQ_CANDIDATO == result_vec[i].first.SQ_CANDIDATO)
            return true;
        }
        return false;
    }


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


    // Inicializa uma tabela com organização heap
    template <typename T>
    MemoryWrapper<DataBlock<T>> initialize_heap(int num_registros, std::string old_vhd, std::string new_vhd) {

        remove(new_vhd.c_str());
        int vhd_name = vhdf::openDisk(new_vhd.c_str(), sizeof(T)*40000, true);
        MemoryWrapper<DataBlock<T>> mem = MemoryWrapper<DataBlock<T>>(vhd_name);
        
        Schema<T> schema = Schema<T>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;

        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        MemoryWrapper<DataBlock<T>> vhd(vhdf::openDisk(old_vhd.c_str()));

        int registrosInseridos = 0;

        // std::cout << "Regs por bloco: " << schema.regs_por_bloco << std::endl;
        // std::cout << "Num_registros: " << num_registros << std::endl;
        // std::cout << "Ceil: " << std::ceil(float(num_registros)/schema.regs_por_bloco) << std::endl;

        // Inserir registros
        for (int i = 1; i <=std::ceil(float(num_registros)/schema.regs_por_bloco); i++) {
            std::vector<T> inserts = std::vector<T>();
            vhd.loadBlock(i);
            for (int j = 0; j < schema.regs_por_bloco; j++) {
                if(registrosInseridos < num_registros) {
                    inserts.push_back(vhd->getRegistro(j));
                    registrosInseridos++;
                }
            }
            // std::cout << "Registros inseridos: " << registrosInseridos << std::endl;
            // std::cout << "Inserts size: " << inserts.size() << std::endl;
            // std::cout << "Inserts vector: ";
            // for(int i=0; i<inserts.size(); ++i)
            //     std::cout << inserts[i].NR_PARTIDO << ' ';
            // std::cout << std::endl;            
            Heap::INSERT(mem, inserts);
        }       
        return mem;
    }

    // Retorna próximo Registro/RegistroPartido
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

    // Printa conteúdos de uma tabela genérica
    template <typename T> 
    void printTable(MemoryWrapper<DataBlock<T>> &mem) {
    }

    // Printa conteúdos de uma tabela de Registro
    template <> 
    void printTable<Registro>(MemoryWrapper<DataBlock<Registro>> &mem) {
        Schema<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "TABELA" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;

        std::cout << "Primeiro bloco: " << schema.primeiro_bloco << std::endl;
        std::cout << "Ultimo bloco: " << schema.ultimo_bloco << std::endl;

        for(int i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
            mem.loadBlock(i);
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << "Bloco " << i << std::endl;
            std::cout << "----------------------------------------------------" << std::endl;
            for(int j=0; j<schema.regs_por_bloco; j++) {
                Registro reg = mem->getRegistro(j);
                if (reg.NR_PARTIDO > 0 && reg.NR_PARTIDO < 100)
                    std::cout << "Registro " << j << ": " << "NR_PARTIDO=" << reg.NR_PARTIDO << "; " << "NM_CANDIDATO=" << reg.NM_CANDIDATO << std::endl;
            }
        }
        std::cout << "----------------------------------------------------" << std::endl;
    }

    // Printa conteúdos de uma tabela de RegistroPartido
    template <> 
    void printTable<RegistroPartido>(MemoryWrapper<DataBlock<RegistroPartido>> &mem) {
        Schema<RegistroPartido> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::cout << "----------------------------------------------------" << std::endl;
        std::cout << "TABELA" << std::endl;
        std::cout << "----------------------------------------------------" << std::endl;

        std::cout << "Primeiro bloco: " << schema.primeiro_bloco << std::endl;
        std::cout << "Último bloco: " << schema.ultimo_bloco << std::endl;

        for(int i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
            mem.loadBlock(i);
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << "Bloco " << i << std::endl;
            std::cout << "----------------------------------------------------" << std::endl;
            for(int j=0; j<schema.regs_por_bloco; j++) {
                RegistroPartido reg = mem->getRegistro(j);
                if (reg.NR_PARTIDO > 0 && reg.NR_PARTIDO < 100)
                    std::cout << "Registro " << j << ": " << "NR_PARTIDO=" << reg.NR_PARTIDO << "; " << "NM_PARTIDO=" << reg.NM_PARTIDO << std::endl;
            }
        }
        std::cout << "----------------------------------------------------" << std::endl;
    }


    // Inicializa uma tabela de num_registros registros ordenada por campo_ordenacao
    template <typename T>
    MemoryWrapper<DataBlock<T>> initialize_ordered(std::string campo_ordenacao, int num_registros, std::string old_vhd, std::string new_vhd) {
        remove(new_vhd.c_str());
        int vhd_ordenado = vhdf::openDisk(new_vhd.c_str(), sizeof(T)*40000, true);
        MemoryWrapper<DataBlock<T>> mem = MemoryWrapper<DataBlock<T>>(vhd_ordenado);
        Schema<T> schema = Schema<T>();
        schema.org = ORDERED;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;

        // Buscar identificador da chave de ordenação
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
        for (int i = 1; i <=std::ceil(float(num_registros)/schema.regs_por_bloco); i++) {
            std::vector<T> inserts = std::vector<T>();
            vhd.loadBlock(i);
            for (int j = 0; j < schema.regs_por_bloco; j++) {
                if(registrosInseridos < num_registros) {
                    inserts.push_back(vhd->getRegistro(j));
                    registrosInseridos++;
                }
            }
            Ordered::INSERT(mem, inserts);
        }
        return mem;
    }



    // Implementa o algoritmo HashJoin
    std::vector<std::pair<Registro, RegistroPartido>> HASHJOIN(MemoryWrapper<DataBlock<Registro>> &mem1, MemoryWrapper<DataBlock<RegistroPartido>> &mem2, std::string join_attribute) {

        std::cout << "Initializing HashJoin..." << std::endl;

        // Inicializar vetor de resultados
        std::vector<std::pair<Registro, RegistroPartido>> ret_regs = std::vector<std::pair<Registro, RegistroPartido>>();

        // Inicializar schemas
        Schema<Registro> schema1;
        Schema<RegistroPartido> schema2;
        vhdf::readBlock(mem1.getDiskId(), 0, &schema1);
        vhdf::readBlock(mem2.getDiskId(), 0, &schema2);

        // Calcular total de registros na relação 1
        mem1.loadBlock(schema1.ultimo_bloco);
        int numRegistros1 = ((schema1.ultimo_bloco-1)*schema1.regs_por_bloco)+mem1->registrosEscritos.count();
        int countRegistros1 = 0;
        // std::cout << "Registros escritos Tabela 1: " << numRegistros1 << std::endl;

        // Calcular total de registros na relação 1
        mem1.loadBlock(schema1.ultimo_bloco);
        int numRegistros2 = ((schema2.ultimo_bloco-1)*schema1.regs_por_bloco)+mem2->registrosEscritos.count();
        int countRegistros2 = 0;
        // std::cout << "Registros escritos Tabela 2: " << numRegistros2 << std::endl;

        // Criar tabela hash
        std::unordered_multimap<int, Registro> hashmap;

        for(int i = schema1.primeiro_bloco; i <= schema1.ultimo_bloco; i++) {
            mem1.loadBlock(i);
            for(int j=0; j<schema1.regs_por_bloco; j++) {
                Registro reg = mem1->getRegistro(j);
                if(countRegistros1 < numRegistros1) {
                    hashmap.insert(std::make_pair(reg.NR_PARTIDO, reg));
                    countRegistros1++;
                }
            }
        }

        std::cout << "Hashmap built from Table 1:" << std::endl;
        // std::cout << "Bucket count: " << hashmap.bucket_count() << std::endl;
        for (auto it = hashmap.begin(); it != hashmap.end(); it++) {
            std::cout << it->first << ": " << it->second.NM_CANDIDATO << std::endl;
        }

        // Iniciar algoritmo de join (compara Tabela 2 com tabela hash)
        for(int i = schema2.primeiro_bloco; i <= schema2.ultimo_bloco; i++) {
            mem2.loadBlock(i);
            for(int j=0; j<schema2.regs_por_bloco; j++) {
                RegistroPartido reg = mem2->getRegistro(j);
                if(countRegistros2 < numRegistros2) {
                    auto range = hashmap.equal_range(reg.NR_PARTIDO);
                    //std::cout << "Found range= " << range.first->first << std::endl;
                    for(auto it=range.first; it!=range.second; it++) {
                        // Match!
                        std::pair<Registro, RegistroPartido> match = std::make_pair(it->second,reg);
                        if(!isResultDuplicate(match, ret_regs)) {
                            ret_regs.push_back(match);
                            //std::cout << it->second.NM_CANDIDATO << "; ";
                        }
                    }
                    //std::cout << std::endl;
                    countRegistros2++;
                }
            }
        } 
        std::cout << "HashJoin finished!" << std::endl;

        return ret_regs;
    }

    // Implementa o algoritmo SortMergeJoin
    std::vector<std::pair<Registro, RegistroPartido>> SORTMERGEJOIN(std::string join_attribute, int num_registros) {

        std::cout << "Initializing SortMergeJoin..." << std::endl;

        // Inicializar vetor de resultados
        std::vector<std::pair<Registro, RegistroPartido>> ret_regs = std::vector<std::pair<Registro, RegistroPartido>>();

        // Inicializar tabelas ordenadas
        MemoryWrapper<DataBlock<Registro>> mem_ordered1 = initialize_ordered<Registro>(join_attribute, num_registros, "testdisk.vhd", "vhd_equijoin_registro.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem_ordered2 = initialize_ordered<RegistroPartido>(join_attribute, num_registros, "partydisk.vhd", "vhd_equijoin_registropartido.vhd");

        // Inicializar schemas
        Schema<Registro> schema1;
        Schema<RegistroPartido> schema2;
        vhdf::readBlock(mem_ordered1.getDiskId(), 0, &schema1);
        vhdf::readBlock(mem_ordered2.getDiskId(), 0, &schema2);

        //Inicializar registros e contadores
        Registro reg1;
        Registro reg1_extra;
        RegistroPartido reg2;
        RegistroPartido reg2_extra;

        mem_ordered1.loadBlock(1);
        mem_ordered2.loadBlock(1);
        reg1 = mem_ordered1->getRegistro(0);
        reg2 = mem_ordered2->getRegistro(0);
        int reg1Count = 0;
        int reg2Count = 0;

        // Inicializar algoritmo de join
        while(reg1.NR_PARTIDO != -1 && reg2.NR_PARTIDO !=-1) {
            if(reg1.NR_PARTIDO > reg2.NR_PARTIDO) {
                std::cout << "reg1 > reg2" << std::endl;
                reg2 = nextRegistro(reg2Count%schema2.regs_por_bloco,(float(reg2Count)/schema2.regs_por_bloco)+1,mem_ordered2);
                reg2Count++;
            } else if (reg1.NR_PARTIDO < reg2.NR_PARTIDO) {
                std::cout << "reg1 < reg2" << std::endl;
                reg1 = nextRegistro(reg1Count%schema1.regs_por_bloco,(float(reg1Count)/schema1.regs_por_bloco)+1,mem_ordered1);
                reg1Count++;
            } else { 
                // Match found
                std::cout << "reg1 = reg2 (Match!)" << std::endl;
                std::pair<Registro, RegistroPartido> result = std::make_pair(reg1,reg2);
                    if(!isResultDuplicate(result,ret_regs)) {
                        ret_regs.push_back(result);

                        // Output further tuples that match with reg1
                        int reg2Count_extra = reg2Count;
                        reg2_extra = nextRegistro(reg2Count_extra%schema2.regs_por_bloco,(float(reg2Count_extra)/schema2.regs_por_bloco)+1,mem_ordered2);
                        while(reg2_extra.NR_PARTIDO != -1 && reg1.NR_PARTIDO == reg2_extra.NR_PARTIDO) {
                            std::cout << "reg1 = reg2' (Match!)" << std::endl;
                            std::pair<Registro, RegistroPartido> result_extra2 = std::make_pair(reg1,reg2_extra);
                            if(!isResultDuplicate(result_extra2,ret_regs))        
                                ret_regs.push_back(result_extra2);
                            reg2Count_extra++;
                            reg2_extra = nextRegistro(reg2Count_extra%schema2.regs_por_bloco,(float(reg2Count_extra)/schema2.regs_por_bloco)+1,mem_ordered2);
                        }
                        // Output further tuples that match with reg2
                        int reg1Count_extra = reg1Count;
                        reg1_extra = nextRegistro(reg1Count_extra%schema1.regs_por_bloco,(float(reg1Count_extra)/schema1.regs_por_bloco)+1,mem_ordered1);
                        while(reg1_extra.NR_PARTIDO != -1 && reg2.NR_PARTIDO == reg1_extra.NR_PARTIDO) {
                            std::cout << "reg1' = reg2 (Match!)" << std::endl;
                            std::pair<Registro, RegistroPartido> result_extra1 = std::make_pair(reg1_extra,reg2);
                            if(!isResultDuplicate(result_extra1,ret_regs))
                                ret_regs.push_back(result_extra1);
                            reg1Count_extra++;
                            reg1_extra = nextRegistro(reg1Count_extra%schema1.regs_por_bloco,(float(reg1Count_extra)/schema1.regs_por_bloco)+1,mem_ordered1);
                        }
                    }
                reg1 = nextRegistro(reg1Count%schema1.regs_por_bloco,(float(reg1Count)/schema1.regs_por_bloco)+1,mem_ordered1);
                reg2 = nextRegistro(reg2Count%schema2.regs_por_bloco,(float(reg2Count)/schema2.regs_por_bloco)+1,mem_ordered2);
                reg1Count++;
                reg2Count++;
            }
        }
        std::cout << "SortMergeJoin finished!" << std::endl;
        return ret_regs;
    }


    // Implementa o algoritmo NestedJoin
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


    void runTests() {
        MemoryWrapper<DataBlock<Registro>> mem1 = initialize_heap<Registro>(13, "testdisk.vhd", "vhd_equijoin_registro_heap.vhd");
        MemoryWrapper<DataBlock<RegistroPartido>> mem2 = initialize_heap<RegistroPartido>(29, "partydisk.vhd", "vhd_equijoin_registropartido_heap.vhd");
        //MemoryWrapper<DataBlock<Registro>> mem_ordered = initialize_ordered<Registro>("NR_PARTIDO", 13, "testdisk.vhd", "vhd_equijoin_registro_ordered.vhd" );

        Schema<Registro> schema1;
        vhdf::readBlock(mem1.getDiskId(), 0, &schema1);

        printTable(mem1);
        printTable(mem2);
        //printTable(mem_ordered);


        try {
            //std::vector<std::pair<Registro, RegistroPartido>> vect;
            //vect = Join::NESTEDJOIN(mem1, mem2, {"NR_PARTIDO=18"});
            //std::cout << vect[0].first.NM_CANDIDATO << " - " << vect.size() << std::endl;
            std::vector<std::pair<Registro, RegistroPartido>> sortmergejoin_results = SORTMERGEJOIN("NR_PARTIDO",13);
            std::cout << "SortMergeJoin results: " << std::endl;
            for(int i = 0; i < sortmergejoin_results.size(); i++) {
               std::cout << sortmergejoin_results[i].first.NR_PARTIDO << ", " << sortmergejoin_results[i].second.NR_PARTIDO << std::endl;
            }
            std::vector<std::pair<Registro, RegistroPartido>> hashjoin_results = HASHJOIN(mem1, mem2,"NR_PARTIDO");
            std::cout << "HashJoin results: " << std::endl;
            for(int i = 0; i < hashjoin_results.size(); i++) {
               std::cout << hashjoin_results[i].first.NR_PARTIDO << ", " << hashjoin_results[i].second.NR_PARTIDO << std::endl;
            }
        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }
        vhdf::closeDisk(mem1.getDiskId());
        vhdf::closeDisk(mem2.getDiskId());
    }

}