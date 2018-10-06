#include <main.hpp>

int main(int argc, char const *argv[])
{
    std::cout << "Tamanho do registro e de " << sizeof(Registro) << " bytes." << std::endl;
    std::cout << "Tamanho do bloco e de " << vhdf::BLOCK_SIZE << " bytes (" << std::floor(vhdf::BLOCK_SIZE/sizeof(Registro)) << " registros por bloco).";
    return 0;
}
