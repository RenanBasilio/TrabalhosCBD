#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <vhdf.hpp>
#include <exception>

#include <Heap.hpp>

namespace Heap
{

    MemoryWrapper<DataBlock> initialize() {
        //mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHeap.vhd", sizeof(Registro)*40000, true));
        int vhd = vhdf::openDisk("testdisk.vhd");
        return MemoryWrapper<DataBlock>(vhd);
    }

    void cleanup(MemoryWrapper<DataBlock> &mem) {
        vhdf::closeDisk(mem.getDiskId());
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
            vect = SELECT(mem, {"NM_CANDIDATO=IDEVANIR ARCANJO DE SOUZA"});
            std::cout << "SELECT NM_CANDIDATO=IDEVANIR ARCANJO DE SOUZA: " << mem.blockAccessCount << std::endl;
            vect = SELECT(mem, {"NM_CANDIDATO=CELIO DA SILVA SANTOS"});
            std::cout << "SELECT NM_CANDIDATO=CELIO DA SILVA SANTOS: " << mem.blockAccessCount << std::endl;
            std::cout << mem.blockAccessCount/6 << std::endl;
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