#include <main.hpp>
#include <fstream>
#include <string>

int main(int argc, char const *argv[])
{
    std::cout << "Tamanho do registro e de " << sizeof(Registro) << " bytes." << std::endl;
    std::cout << "Tamanho do bloco e de " << vhdf::BLOCK_SIZE << " bytes (" << std::floor(vhdf::BLOCK_SIZE/sizeof(Registro)) << " registros por bloco).";
    
    std::cout << "Initializing virtual hard disk... ";
    int vhd = vhdf::openDisk("testdisk.vhd", sizeof(Registro)*10, true);
    if (vhd == -1) {
        std::cout << "Error opening virtual hard disk." << std::endl;
        return -1;
    }
    std::cout << "Success" << std::endl;

    std::cout << "Parsing input data from file consulta_cand_2018_BRASIL.csv..." << std::endl;
    std::ifstream ifs;
    ifs.open("consulta_cand_2018_BRASIL.csv", std::ifstream::in);
    std::string str;
    std::getline(ifs, str);
    for (int i = 0; i < 4; i++) {
        std::getline(ifs, str);
        Registro reg(str);
        char block[vhdf::BLOCK_SIZE];
        vhdf::readBlock(vhd, 0, block);
        setRegistro(0, block, reg);
        vhdf::writeBlock(vhd, 0, block);
    }
    ifs.close();
    vhdf::closeDisk(vhd);

    
    return 0;
}
