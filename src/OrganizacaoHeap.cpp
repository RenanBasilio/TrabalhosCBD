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

namespace OrganizacaoHeap
{
    int vhd;
    MemoryWrapper<DataBlock> mem;

    void initialize() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        vhd = vhdf::openDisk("testdisk.vhd", sizeof(Registro)*40000, true);
        mem = MemoryWrapper<DataBlock>(vhd);
    }

    void cleanup() {
        vhdf::closeDisk(vhd);
    }

    // Considere cada argumento extra no formato "CAMPO=valor"
    std::vector<Registro> select(std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs;

        std::vector<std::pair<Campo, std::string>> targets;
        //std::cout << "Parsing select fields..." << std::endl;

        for (int i = 0; i < params.size(); i++) {
            std::string str = params[i];
            unsigned int pos = str.find('=');
            std::string campo = str.substr(0, pos);
            std::string valor = str.substr(pos+1, str.length() - pos);
            bool found = false;

            for (int j = 0; j < schema.nCampos; j++) {
                if (strcmp(schema.campos[j].nm_campo, campo.c_str()) == 0) {
                    targets.push_back({schema.campos[j], valor});
                    found = true;
                }
            }
            if (!found) {
                throw std::invalid_argument("O campo "+campo+" nao consta da tabela");
            }
        }

        Registro reg;
        
        //std::cout << "Comparing against database members..." << std::endl;
        for (size_t i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
            mem.loadBlock(i);
            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                reg = mem->getRegistro(j);

                int match = 0;
                for (int k = 0; k < targets.size(); k++) {
                    void* cmp_ptr = nullptr;
                    switch (targets[k].first.tipo) {
                        case INT:
                            cmp_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
                            if (*reinterpret_cast<int*>(cmp_ptr) == std::stoi(targets[k].second)) match++;
                            break;
                        case BIGINT:
                            cmp_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
                            if (*reinterpret_cast<size_t*>(cmp_ptr) == std::atoll(targets[k].second.c_str())) match++;
                            break;
                        case CHAR:
                            cmp_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
                            if ( strcmp(reinterpret_cast<char*>(cmp_ptr), targets[k].second.c_str()) == 0) match++;
                            break;
                        /*
                        case DATA: {
                            Data* data_ptr = reinterpret_cast<Data*>(&reg + targets[k].first.pos_relativa);
                            if (*data_ptr == Data(targets[k].second)) match++;
                            break;
                        }
                        case HORA: {
                            Data* hora_ptr = reinterpret_cast<Hora*>(&reg + targets[k].first.pos_relativa);
                            if (*hora_ptr == Hora(targets[k].second)) match++;
                            break;
                        }
                        case TIMESTAMP: {
                            Timestamp* tstamp_ptr = reinterpret_cast<Timestamp*>(&reg + targets[k].first.pos_relativa);
                            if (*tstamp_ptr == Timestamp(targets[k].second)) match++;
                            break;
                        }
                        */
                        case BOOL:
                            cmp_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
                            if (*reinterpret_cast<bool*>(cmp_ptr) == SNtoBool(targets[k].second)) match++;
                            break;
                    }
                }
                if (match == targets.size()) {
                    ret_regs.push_back(reg);
                    //std::cout << "Found match " << reg.NM_CANDIDATO << std::endl;
                }
            }
        }

        return ret_regs;
    }


    void runTests() {
        initialize();
        try {
            std::vector<Registro> vect = select({"ANO_ELEICAO=2018", "NR_CANDIDATO=12"});
            vect = select({"ST_REELEICAO=S"});
            vect = select({"CD_SIT_TOT_TURNO=-1"});
            vect = select({"NR_PROCESSO=06017561520186160000"});
            //std::vector<Registro> vect = select({"ANO_ELEICAO=2018", "CD_TIPO_ELEICAO=2", "NR_TURNO=1", "CD_ELEICAO=297"});
            std::cout << "It WORKS!" << std::endl;
        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }
        cleanup();
    };
}