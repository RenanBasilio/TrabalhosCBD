//
// Created by Tuniks on 11/9/2018.
//

#include <ParserPartido.hpp>
#include <cmath>
#include <cstring>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <Schema.hpp>

void parsePartyStream(std::ifstream& ifs, int vhd, int blockoffset) {
    std::string str;
    size_t blocksprocessed = blockoffset;
    size_t regsprocessed = 0;

    MemoryWrapper<DataBlock> mem(vhd);
    mem.loadBlock(blocksprocessed);

    Schema<RegistroPartido> schema = Schema<RegistroPartido>();
    schema.primeiro_bloco = blockoffset;

    while(getline(ifs, str)) {
        // Se o numero de registros por bloco foi atingido, comitta no vhd
        if (regsprocessed == RegistroPartido::nPorBloco()) {
            mem.commitBlock();
            regsprocessed = 0;
            blocksprocessed++;
            mem.loadBlock(blocksprocessed);
        }
        RegistroPartido reg(str);
        mem->setRegistro(regsprocessed, reg);
        regsprocessed++;
    }
    mem.commitBlock();
    blocksprocessed++;

    schema.ultimo_bloco = blocksprocessed;
    vhdf::writeBlock(vhd, 0, &schema);
}