#pragma once
#include <MemoryWrapper.hpp>
#include <Util.hpp>

MemoryWrapper createTable ( const std::string &nome, size_t maxBytes, std::vector<Campo> campos, Organizacao org, std::string campo_ordenacao ) {
    remove(nome.c_str());
    int vhd = vhdf::openDisk(nome.c_str(), maxBytes, true);

    Schema schema(campos);
    schema.primeiro_bloco = 1;
    schema.ultimo_bloco = 1;
    schema.org = org;
    schema.blocos_hash = 0;
    schema.chave = 0;
    for ( size_t i = 0; i < campos.size(); i++ ) {
        if ( std::string(campos.at(i).nm_campo) == campo_ordenacao ) schema.chave = i;
    }

    MemoryWrapper mem(vhd, schema);

    return mem;
}

MemoryWrapper createTable ( const std::string &nome, size_t maxBytes, std::vector<Campo> campos, Organizacao org, int hash_bins ) {
    remove(nome.c_str());
    int vhd = vhdf::openDisk(nome.c_str(), maxBytes, true);

    Schema schema(campos);
    schema.primeiro_bloco = 1;
    schema.ultimo_bloco = 1 + hash_bins;
    schema.org = org;
    schema.blocos_hash = hash_bins;
    schema.chave = 0;

    MemoryWrapper mem(vhd, schema);

    return mem;
}

MemoryWrapper createTable ( const std::string &nome, size_t maxBytes, std::vector<Campo> campos, Organizacao org ) {
    remove(nome.c_str());
    int vhd = vhdf::openDisk(nome.c_str(), maxBytes, true);

    Schema schema(campos);
    schema.primeiro_bloco = 1;
    schema.ultimo_bloco = 1;
    schema.org = org;
    schema.blocos_hash = 0;
    schema.chave = 0;

    MemoryWrapper mem(vhd, schema);

    return mem;
}