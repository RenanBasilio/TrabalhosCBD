#include <Parser.hpp>
#include <cmath>
#include <cstring>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>

void parseStream(std::ifstream& ifs, int vhd, int blockoffset) {
    std::string str;
    size_t blocksprocessed = blockoffset;
    size_t regsprocessed = 0;

    MemoryWrapper<DataBlock> mem(vhd);
    mem.loadBlock(blocksprocessed);

    while(getline(ifs, str)) {
        // Se o numero de registros por bloco foi atingido, comitta no vhd
        if ((regsprocessed % Registro::nPorBloco()) == 0) {
            mem.commitBlock();
            regsprocessed = 0;
            blocksprocessed++;
            mem.loadBlock(blocksprocessed);
        }
        Registro reg(str);
        mem->setRegistro(regsprocessed, reg);
    }
    mem.commitBlock();
}