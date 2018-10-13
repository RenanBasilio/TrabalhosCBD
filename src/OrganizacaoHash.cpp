#include <iostream>
#include <Campo.hpp>
#include <vhdf.hpp>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <Registro.hpp>

namespace OrganizacaoHash
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

    void runTests() {
        initialize();
        /*try {
            std::vector<Registro> vect;
            //vect= select({"ANO_ELEICAO=2018", "NR_CANDIDATO=12"});
            vect = select({"ST_REELEICAO=S"});
            vect = select({"ST_DECLARAR_BENS=S"});
            //vect = select({"CD_SIT_TOT_TURNO=-1"});
            vect = select({"NR_PROCESSO=06017561520186160000"});
            //std::vector<Registro> vect = select({"ANO_ELEICAO=2018", "CD_TIPO_ELEICAO=2", "NR_TURNO=1", "CD_ELEICAO=297"});
            std::cout << "It WORKS!" << std::endl;
        }
        catch (std::invalid_argument e) {
            std::cout << "Erro: " << e.what() << std::endl;
        }*/
        cleanup();
    };

}