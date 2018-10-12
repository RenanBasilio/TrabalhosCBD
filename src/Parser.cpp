#include <Parser.hpp>
#include <cmath>
#include <cstring>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <HEAD.hpp>

void parseStream(std::ifstream& ifs, int vhd, int blockoffset) {
    std::string str;
    size_t blocksprocessed = blockoffset;
    size_t regsprocessed = 0;

    MemoryWrapper<DataBlock> mem(vhd);
    mem.loadBlock(blocksprocessed);

    HEAD<Registro> schema = HEAD<Registro>();
    schema.primeiro_bloco = blockoffset;

    while(getline(ifs, str)) {
        // Se o numero de registros por bloco foi atingido, comitta no vhd
        if (regsprocessed == Registro::nPorBloco()) {
            mem.commitBlock();
            regsprocessed = 0;
            blocksprocessed++;
            mem.loadBlock(blocksprocessed);
        }
        Registro reg(str);
        mem->setRegistro(regsprocessed, reg);
        regsprocessed++;
    }
    mem.commitBlock();
    blocksprocessed++;

    schema.ultimo_bloco = blocksprocessed;
    vhdf::writeBlock(vhd, 0, &schema);
}