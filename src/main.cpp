#include <main.hpp>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>

#include <Schema.hpp>
#include <Hash.hpp>
#include <Ordered.hpp>
#include <Heap.hpp>
#include <Equijoin.hpp>
#include <RegistroPartido.hpp>
#include <Index.hpp>

int main(int argc, char const *argv[])
{
    if (Registro::nPorBloco() < 0) throw std::runtime_error("Erro: Tamanho do registro é grande de mais (>1 bloco)");
    
    std::cout << "Tamanho do registro e de " << sizeof(Registro) << " bytes." << std::endl;
    std::cout << "Tamanho do bloco e de " << vhdf::BLOCK_SIZE << " bytes (" << Registro::nPorBloco() << " registros por bloco)." << std::endl;
    
    std::cout << "Initializing virtual hard disk... ";
    remove("testdisk.vhd");
    int vhd = vhdf::openDisk("testdisk.vhd", sizeof(Registro)*40000, true);
    if (vhd == -1) {
        std::cout << "Error opening virtual hard disk." << std::endl;
        return -1;
    }
    std::cout << "Success" << std::endl;

    //inicializando disco com a tabela de partidos
    remove("partydisk.vhd");
    int partyvhd = vhdf::openDisk("partydisk.vhd", sizeof(RegistroPartido)*40000, true);
    if (partyvhd == -1) {
        std::cout << "Error opening virtual hard disk." << std::endl;
        return -1;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Parsing input data from file consulta_cand_2018_BRASIL.csv...";
    std::ifstream ifs;
    ifs.open("consulta_cand_2018_BRASIL.csv", std::ifstream::in);
    std::string str;
    std::getline(ifs, str);
    parseStream<Registro>(ifs, vhd, 1, 40000);
    ifs.clear();
    ifs.seekg(0, std::ios::beg);
    std::getline(ifs, str);
    parseStream<RegistroPartido>(ifs, partyvhd, 1, 40000);     //tabela de partidos

    MemoryWrapper<DataBlock<Registro>> mem = MemoryWrapper<DataBlock<Registro>>(vhd);
/*     Index index = createIndex("testdisk.vhd", mem, "NM_CANDIDATO", ORDERED);
    std::vector<size_t> blocks = index.findBlocks("ABEL COSTA"); */

    std::cout << "Executando testes..." << std::endl; 
    std::cout << "Formato: [Nome do Teste: número de acessos a bloco]" << std::endl;
    std::cout << "Armazenamento heap... " << std::endl; 
    //Heap::runTests();
    std::cout << "Done" << std::endl; 
    std::cout << "Armazenamento ordenado..." << std::endl;
    //Ordered::runTests();
    std::cout << "Done" << std::endl;
    std::cout << "Armazenamento em hash..." << std::endl;
    //Hash::runTests();
    std::cout << "Done" << std::endl;

    std::cout << "Testes de Join" << std::endl;
    Join::runTests();
    std::cout << "Done" << std::endl;

    ifs.close();
    vhdf::closeDisk(vhd);

    system("pause");
    return 0;
}
