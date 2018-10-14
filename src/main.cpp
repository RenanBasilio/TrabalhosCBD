#include <main.hpp>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>

#include <HEAD.hpp>
#include <OrganizacaoHash.hpp>
#include <OrganizacaoOrdenada.hpp>
#include <OrganizacaoHeap.hpp>


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

    std::cout << "Parsing input data from file consulta_cand_2018_BRASIL.csv...";
    std::ifstream ifs;
    ifs.open("consulta_cand_2018_BRASIL.csv", std::ifstream::in);
    std::string str;
    std::getline(ifs, str);
    parseStream(ifs, vhd, 1);
    std::cout << "Done" << std::endl;

    std::cout << "Executando testes..." << std::endl;
    std::cout << "Armazenamento heap... "; 
    //OrganizacaoHeap::runTests();
    std::cout << "Done" << std::endl << "Armazenamento ordenado...";
    //OrganizacaoOrdenada::runTests();
    std::cout << "Done" << std::endl << "Armazenamento em hash...";
    OrganizacaoHash::runTests();
    std::cout << "Done" << std::endl;


    ifs.close();
    vhdf::closeDisk(vhd);

    return 0;
}
