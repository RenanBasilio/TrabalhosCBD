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

namespace OrganizacaoHeap
{

    MemoryWrapper<DataBlock> initialize() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("testdisk.vhd");
        return MemoryWrapper<DataBlock>(vhd);
    }

    void cleanup(MemoryWrapper<DataBlock> &mem) {
        vhdf::closeDisk(mem.getDiskId());
    }

    bool INSERT(MemoryWrapper<DataBlock> &mem, std::vector<Registro> registros) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        size_t pos_inicial = schema.ultimo_bloco;

        mem.loadBlock(pos_inicial);
        int blocos_processados = 0;
        int regs_processados = mem->getPrimeiroRegistroDispEscrita();
        bool block_changed = false;

        for (int i = 0; i<registros.size(); i++) {
            if (regs_processados == schema.regs_por_bloco) {
                if (block_changed) mem.commitBlock();
                blocos_processados++;
                mem.loadBlock(pos_inicial+blocos_processados);
                if (mem.getLoadedBlockId() > schema.ultimo_bloco) mem->initialize();
                block_changed = false;
                regs_processados = mem->getPrimeiroRegistroDispEscrita();
            }
            if (!mem->isRegistroEscrito(regs_processados)) {
                mem->setRegistro(regs_processados, registros[i]);
                block_changed = true;
            }
            else {
                regs_processados = mem->getPrimeiroRegistroDispEscrita();
                while (regs_processados == -1) {
                    if (block_changed) mem.commitBlock();
                    blocos_processados++;
                    mem.loadBlock(pos_inicial+blocos_processados);
                    if (mem.getLoadedBlockId() > schema.ultimo_bloco) mem->initialize();
                    block_changed = false;
                    regs_processados = mem->getPrimeiroRegistroDispEscrita();
                }
            }
        }
        mem.commitBlock();

        if (schema.ultimo_bloco != pos_inicial+blocos_processados) {
            schema.ultimo_bloco = pos_inicial+blocos_processados;
            vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        }

        return true;
    }

    // Um select de comparacao simples pode ser feito usando params do tipo {"CAMPO=valor"}
    // Um select de comparacao em uma faixa pode ser feito usando params do tipo {"CAMPO=[min:max]"}
    // Um select de comparacao em um conjunto de valores pode ser feito usando params do tipo {"CAMPO={valor1,valor2}"}
    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs = std::vector<Registro>();

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        
        //std::cout << "Comparing against database members..." << std::endl;
        for (size_t i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
            mem.loadBlock(i);
            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                reg = mem->getRegistro(j);

                int match = 0;
                for (int k = 0; k < targets.size(); k++) {
                    switch (targets[k].tipo) {
                        case VALUE:
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0])) match++;
                            break;
                        case SET:
                            for (int v = 0; v < targets[k].valor.size(); v++)
                                if (comparaCampo(targets[k].campo, &reg, targets[k].valor[v])) {
                                    match++;
                                    break;
                                }
                            break;
                        case RANGE:
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0], ">=") && 
                                comparaCampo(targets[k].campo, &reg, targets[k].valor[1], "<=")) match++;
                            break;
                        default:
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

    bool DELETE(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params) {
        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs;

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        
        //std::cout << "Comparing against database members..." << std::endl;
        for (size_t i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
            mem.loadBlock(i);
            bool changed = false;
            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                reg = mem->getRegistro(j);

                int match = 0;
                for (int k = 0; k < targets.size(); k++) {
                    switch (targets[k].tipo) {
                        case VALUE:
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0])) match++;
                            break;
                        case SET:
                            for (int v = 0; v < targets[k].valor.size(); v++)
                                if (comparaCampo(targets[k].campo, &reg, targets[k].valor[v])) {
                                    match++;
                                    break;
                                }
                            break;
                        case RANGE:
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0], ">=") && 
                                comparaCampo(targets[k].campo, &reg, targets[k].valor[1], "<=")) match++;
                            break;
                        default:
                            break;
                    }
                }
                if (match == targets.size()) {
                    mem->deleteRegistro(j);
                    changed = true;
                    //std::cout << "Found match " << reg.NM_CANDIDATO << std::endl;
                }
            }
            if (changed) mem.commitBlock();
        }
        return true;
    }


    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();
        try {

            
            //INSERT UNICO TEST
            std::vector<Registro> vect;
            Registro reg;
            strncpy(reg.NM_CANDIDATO, "HELLO WORLD", sizeof(reg.NM_CANDIDATO));
            INSERT(mem, {reg});
            std::cout << "INSERT único: " << mem.blockAccessCount << std::endl;
            mem.blockAccessCount = 0;

            //INSERT DUPLO TEST
            std::vector<Registro> reglist(20);
            for(int i=0; i < 20; i++){
                reglist[i].NR_PARTIDO = i;
            }
            INSERT(mem, reglist);
            std::cout << "INSERT duplo: " << mem.blockAccessCount << std::endl;
            mem.blockAccessCount = 0;

            //SELECT UM VALOR TEST
            vect = SELECT(mem, {"NM_CANDIDATO=HELLO WORLD"});
            std::cout << "SELECT NM_CANDIDATO=HELLO WORLD: "<< mem.blockAccessCount << std::endl;
            vect = SELECT(mem, {"NM_CANDIDATO=AUGUSTO LUIZ DE LIMA"});
            std::cout << "SELECT NM_CANDIDATO=AUGUSTO LUIZ DE LIMA: "<< mem.blockAccessCount << std::endl;
            vect = SELECT(mem, {"NM_CANDIDATO=EDVALDO MOREIRA DE SINTRA"});
            std::cout << "SELECT NM_CANDIDATO=EDVALDO MOREIRA DE SINTRA: " << mem.blockAccessCount << vect[0].NM_URNA_CANDIDATO << std::endl;
            vect = SELECT(mem, {"NM_CANDIDATO=ODAIR JOSE FERREIRA"});
            std::cout << "SELECT NM_CANDIDATO=ODAIR JOSE FERREIRA :" << mem.blockAccessCount << std::endl;
            std::cout << mem.blockAccessCount/4 << std::endl;
            mem.blockAccessCount = 0;

            //SELECT MULTIPLOS REGISTROS
            vect = SELECT(mem, {"NM_CANDIDATO={HELLO WORLD,CIRO FERREIRA GOMES}"});
            std::cout << "SELECT NM_CANDIDATO={HELLO WORLD,CIRO FERREIRA GOMES} :" << mem.blockAccessCount << vect[0].NM_CANDIDATO << vect[1].NM_CANDIDATO <<std::endl;
            mem.blockAccessCount = 0;

            //SELECT FAIXA
            vect = SELECT(mem, {"NR_PARTIDO=[10:15]"});
            std::cout << "SELECT NR_PARTIDO=[10:15]: " << mem.blockAccessCount << vect[0].NR_PARTIDO << std::endl;
            mem.blockAccessCount = 0;

            //SELECT 2 VALORES FIXOS
            vect = SELECT(mem, {"NR_PARTIDO=12","NM_CANDIDATO=CIRO FERREIRA GOMES"});
            std::cout << "SELECT NR_PARTIDO=12,NM_CANDIDATO=CIRO FERREIRA GOMES: " <<  mem.blockAccessCount << vect[0].NR_PARTIDO << std::endl;
            mem.blockAccessCount = 0;

            //DELETE UM REG
            DELETE(mem, {"NM_CANDIDATO=HELLO WORLD"});
            std::cout << "DELETE NM_CANDIDATO=HELLO WORLD: " << mem.blockAccessCount << std::endl;
            mem.blockAccessCount = 0;

            //DELETE MULTIPLOS REG
            DELETE(mem, {"NM_CANDIDATO={AUGUSTO LUIZ DE LIMA,ODAIR JOSE FERREIRA}"});
            std::cout << "DELETE NM_CANDIDATO={AUGUSTO LUIZ DE LIMA,ODAIR JOSE FERREIRA}: " << mem.blockAccessCount << std::endl;
            mem.blockAccessCount = 0;

            //DELETE FAIXA
            DELETE(mem, {"NR_PARTIDO=[10:12]"});
            std::cout << "DELETE NR_PARTIDO=[10:12]: " << mem.blockAccessCount << std::endl;
            mem.blockAccessCount = 0;

            //DELETE 2 VALORES FIXOS
            DELETE(mem, {"NR_PARTIDO=12","NM_CANDIDATO=CIRO FERREIRA GOMES"});
            std::cout << "DELETE NR_PARTIDO=12,NM_CANDIDATO=CIRO FERREIRA GOMES: " << mem.blockAccessCount << std::endl;
            mem.blockAccessCount = 0;



            //vect= select({"ANO_ELEICAO=2018", "NR_CANDIDATO=12"});
            //vect = select({"ST_REELEICAO=S"});
            //vect = select({"ST_DECLARAR_BENS=S"});
            //vect = select({"CD_SIT_TOT_TURNO=-1"});
            //vect = select({"NR_PROCESSO=06017561520186160000"});
            //vect = select({"NR_PROCESSO={06017561520186160000,06016366920186160000}"});
            //vect = select({"NR_CANDIDATO=[10:20]"});
            //vect = select({"ANO_ELEICAO=2018", "CD_TIPO_ELEICAO=2", "NR_TURNO=1", "CD_ELEICAO=297"});
            std::cout << "It WORKS!" << std::endl;
        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }
        cleanup(mem);
    };
}