#include <string>
#include <cstring>
#include <map>
#include <exception>
#include <algorithm>
#include <vhdf.hpp>

#include <Ordered.hpp>

namespace Ordered
{
    MemoryWrapper<DataBlock<Registro>> initialize() {
        remove("VHDOrdenado.vhd");
        MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhdf::openDisk("VHDOrdenado.vhd", sizeof(Registro)*40000, true));
        Schema<Registro> schema = Schema<Registro>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
        schema.chave = 9;   // SQ_CANDIDATO
        //schema.chave = 11; // NM_CANDIDATO
        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void cleanup(MemoryWrapper<DataBlock<Registro>> mem) {
        vhdf::closeDisk(mem.getDiskId());
    }


    void runTests() {
        MemoryWrapper<DataBlock<Registro>> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock<Registro>> vhd(vhdf::openDisk("testdisk.vhd"));
        std::vector<Registro> vect;

        // Teste com 1000 inserts
        inserts.clear();
        for (int i = 0; i < 2000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);


        //INSERT UNICO TEST
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



        /*

        //INSERT UNICO TEST
        Registro reg;
        strncpy(reg.NM_CANDIDATO, "HELLO WORLD", sizeof(reg.NM_CANDIDATO));
        INSERT(mem, {reg});

        //INSERT DUPLO TEST
        std::vector<Registro> reglist(2);
        strncpy(reglist[0].NM_CANDIDATO, "ROBSON", sizeof(reg.NM_CANDIDATO));
        strncpy(reglist[1].NM_CANDIDATO, "ANDERSON", sizeof(reg.NM_CANDIDATO));
        INSERT(mem, reglist);

        //SELECT UM VALOR TEST
        vect = SELECT(mem, {"NM_CANDIDATO=HELLO WORLD"});

        //SELECT MULTIPLOS REGISTROS
        vect = SELECT(mem, {"NM_CANDIDATO={HELLO WORLD,MARIA DO SOCORRO NASCIMENTO BARBOSA}"});


        //SELECT FAIXA
        vect = SELECT(mem, {"NM_CANDIDATO=[MARIA DO CARMO OLIVEIRA NAFALSKI:MARIA DO SOCORRO NASCIMENTO BARBOSA]"});

        //SELECT 2 VALORES FIXOS
        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});

        //DELETE UM REG
        DELETE(mem, {"NM_CANDIDATO=HELLO WORLD"});

        //DELETE MULTIPLOS REG
        DELETE(mem, {"NM_CANDIDATO={robson,MARIA DO CARMO OLIVEIRA NAFALSKI}"});

       //DELETE FAIXA
       DELETE(mem, {"NR_PARTIDO=[10:12]"});
       //std::cout << mem.blockAccessCount << std::endl;
       //mem.blockAccessCount = 0;

        //DELETE 2 VALORES FIXOS
        DELETE(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});

        */


//        //Teste Select
//        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});
//        std::cout << mem.blockAccessCount << std::endl << vect[0].NM_CANDIDATO << std::endl;
//
//        DELETE(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA,MARIA DO CARMO OLIVEIRA NAFALSKI","ANO_ELEICAO=[2015:2020]"});
//
//        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA,MARIA DO CARMO OLIVEIRA NAFALSKI"});
//        std::cout << mem.blockAccessCount << std::endl << vect[0].NM_CANDIDATO << std::endl;
//        //Teste Select Range
//        vect = SELECT(mem, {"NM_CANDIDATO=[MARIA DO CARMO OLIVEIRA NAFALSKI:MARIA DO SOCORRO NASCIMENTO BARBOSA]","ANO_ELEICAO=[2015:2018]"});
//        for(int i=0; i < vect.size(); i++){
//           std::cout << vect[i].NM_CANDIDATO << std::endl;
//        }
        //Teste Multiplos Registros
//        vect = SELECT(mem, {"NM_CANDIDATO={MARIA DO CARMO OLIVEIRA NAFALSKI,MARIA DO SOCORRO NASCIMENTO BARBOSA}","ANO_ELEICAO=[2015:2020]"});
//        for(int i=0; i < vect.size(); i++){
//            std::cout << vect[i].NM_CANDIDATO << std::endl;
//        }

        vhdf::closeDisk(vhd.getDiskId());

        cleanup(mem);
    };

}