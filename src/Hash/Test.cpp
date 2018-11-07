#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <exception>
#include <algorithm>
#include <vhdf.hpp>

#include <Hash.hpp>

namespace Hash
{
    MemoryWrapper<DataBlock> initialize() {
        remove("VHDHash.vhd");
        MemoryWrapper<DataBlock> mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHash.vhd", sizeof(Registro)*40000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 2000;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
        //schema.chave = 9;   // SQ_CANDIDATO
        schema.chave = 11; // NM_CANDIDATO

        for (int i = 1; i < schema.blocos_hash+1; i++) {
            mem.loadBlock(i);
            mem->initialize();
            mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void cleanup(MemoryWrapper<DataBlock> mem) {
        vhdf::closeDisk(mem.getDiskId());
    }

    size_t calculateHash(Campo campo, std::string chave, size_t tamanho) {
        switch (campo.tipo) {
            case INT:
                return hashingFunc(tamanho, std::stoi(chave));
            case BIGINT:
                return hashingFunc(tamanho, std::atoll(chave.c_str()));
            case CHAR:
                return hashingFunc(tamanho, chave);
            default:
                throw std::invalid_argument("O campo"+std::string(campo.nm_campo, sizeof(campo.nm_campo))+"nao e uma chave valida.");
        }
    }

    
    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        std::vector<Registro> vect;
 
        // Teste com um único insert
        mem.blockAccessCount = 0;
        vhd.loadBlock(1);
        inserts.push_back(vhd->getRegistro(1));
        INSERT(mem, inserts);
        std::cout << "INSERT 1 registro: " << mem.blockAccessCount << std::endl;

        // Teste com 10 inserts
        mem.blockAccessCount = 0;
        inserts.clear();
        vhd.loadBlock(1);
        for (int i = 0; i < 10; i++) {
            inserts.push_back(vhd->getRegistro(i));
        }
        INSERT(mem, inserts);
        std::cout << "INSERT 10 Registros: " << mem.blockAccessCount << std::endl;

        
        // Teste com 20k inserts
        mem.blockAccessCount = 0;
        inserts.clear();
        for (int i = 0; i < 2000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);
        std::cout << "INSERT 20K Registros: " << mem.blockAccessCount << std::endl;
        
      
        /*
        //SELECT UM VALOR TEST (SQ_CANDIDATO)
        mem.blockAccessCount = 0;
        vect = SELECT(mem, {"SQ_CANDIDATO=280000605589"});
        //std::cout << mem.blockAccessCount << std::endl;
        vect = SELECT(mem, {"SQ_CANDIDATO=80000624525"});
        //std::cout << mem.blockAccessCount << std::endl;
        std::cout << "Média (SELECT SQ_CANDIDATO=280000605589 E SELECT SQ_CANDIDATO=80000624525): " << mem.blockAccessCount/2 << std::endl;
        mem.blockAccessCount = 0;

        //SELECT MULTIPLOS REGISTROS SQ_CANDIDATO
        vect = SELECT(mem, {"SQ_CANDIDATO={280000605589,80000624525}"});
        std::cout << "SELECT SQ_CANDIDATO={280000605589,80000624525} " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;

        //SELECT FAIXA SQ_CANDIDATO
        vect = SELECT(mem, {"SQ_CANDIDATO=[280000605589:80000624525]"});
        std::cout << "SELECT SQ_CANDIDATO=[280000605589:80000624525] " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;
    
        //SELECT 2 VALORES FIXOS
        vect = SELECT(mem, {"NR_PARTIDO=12","SQ_CANDIDATO=280000605589"});
        std::cout << "SELECT NR_PARTIDO=12,SQ_CANDIDATO=280000605589 : " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;

        //DELETE UM REG
        DELETE(mem, {"SQ_CANDIDATO=280000605589"});
        std::cout << "DELETE SQ_CANDIDATO=280000605589: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE MULTIPLOS REG
        DELETE(mem, {"SQ_CANDIDATO={280000605589,80000624525}"});
        std::cout << "DELETE SQ_CANDIDATO={280000605589,80000624525}: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE FAIXA
        DELETE(mem, {"NR_PARTIDO=[10:12]"});
        std::cout << "DELETE NR_PARTIDO=[10:12]: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE 2 VALORES FIXOS
        DELETE(mem, {"NR_PARTIDO=12","SQ_CANDIDATO=280000605589"});
        std::cout << "DELETE NR_PARTIDO=12,SQ_CANDIDATO=280000605589: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;
        */



        //SELECT UM VALOR TEST (NM_CANDIDATO)
        mem.blockAccessCount = 0;
        vect = SELECT(mem, {"NM_CANDIDATO=CIRO FERREIRA GOMES"});
        //std::cout << mem.blockAccessCount << std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=AUGUSTO LUIZ DE LIMA"});
        //std::cout << mem.blockAccessCount << std::endl;
        std::cout << "Média (SELECT NM_CANDIDATO=CIRO FERREIRA GOMES E SELECT NM_CANDIDATO=AUGUSTO LUIZ DE LIMA): " << mem.blockAccessCount/2 << std::endl;
        mem.blockAccessCount = 0;

        //SELECT MULTIPLOS REGISTROS NM_CANDIDATO
        vect = SELECT(mem, {"NM_CANDIDATO={HELLO WORLD,CIRO FERREIRA GOMES}"});
        std::cout << "SELECT NM_CANDIDATO={HELLO WORLD,CIRO FERREIRA GOMES}: " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;

        //SELECT FAIXA
        vect = SELECT(mem, {"NR_PARTIDO=[10:12]"});
        std::cout << "SELECT NR_PARTIDO=[10:12] :" << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;

        //SELECT 2 VALORES FIXOS
        vect = SELECT(mem, {"NR_PARTIDO=12","NM_CANDIDATO=CIRO FERREIRA GOMES"});
        std::cout << "SELECT NR_PARTIDO=12,NM_CANDIDATO=CIRO FERREIRA GOMES: " << mem.blockAccessCount <<std::endl;
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



        cleanup(mem);
    };

}