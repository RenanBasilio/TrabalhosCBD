#include <Parser.hpp>

void parseStream(std::ifstream& ifs, int vhd, int blockoffset) {
    std::string str;
    char block[vhdf::BLOCK_SIZE];
    size_t blocksprocessed = blockoffset;
    size_t regsprocessed = 0;
    const size_t regsperblock = std::floor(vhdf::BLOCK_SIZE/sizeof(Registro));

    while(getline(ifs, str)) {
        // Se o numero de registros por bloco foi atingido, comitta no vhd
        if ((regsprocessed % regsperblock) == 0) {
            vhdf::writeBlock(vhd, blocksprocessed, block);
            memset(block, 0x0, vhdf::BLOCK_SIZE);
            regsprocessed = 0;
            blocksprocessed++;
        }
        Registro reg(str);
        setRegistro(regsprocessed, block, reg);
    }
    vhdf::writeBlock(vhd, blocksprocessed, block);
}