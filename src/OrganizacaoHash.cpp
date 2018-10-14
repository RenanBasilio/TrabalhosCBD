#include <iostream>
#include <string>
#include <cstring>
#include <map>
#include <HEAD.hpp>
#include <Registro.hpp>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <vhdf.hpp>
#include <Campo.hpp>
#include <exception>
#include <Query.hpp>
#include <algorithm>
#include <OrganizacaoHash.hpp>
#include <OrganizacaoHeap.hpp>

namespace OrganizacaoHash
{
    MemoryWrapper<DataBlock> initialize() {
        remove("VHDHash.vhd");
        MemoryWrapper<DataBlock> mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDHash.vhd", sizeof(Registro)*40000, true));
        HEAD<Registro> schema = HEAD<Registro>();
        schema.org = HASH;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1000;
        schema.blocos_hash = schema.ultimo_bloco - schema.primeiro_bloco;
        //schema.chave = 9;   // SQ_CANDIDATO
        schema.chave = 11; // NM_CANDIDATO
        mem.loadBlock(1);
        mem->initialize();
        mem.commitBlock();
        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        return mem;
    }

    void cleanup(MemoryWrapper<DataBlock> mem) {
        vhdf::closeDisk(mem.getDiskId());
    }

    size_t calculateHash(Campo campo, std::string chave, size_t tamanho) {
        switch (campo.tipo) {
            case INT:
                return hashingFunc(tamanho, std::stoi(chave));
            case BIGINT:
                return hashingFunc(tamanho, std::atoll(chave.c_str()));
            case CHAR:
                return hashingFunc(tamanho, chave);
            default:
                throw std::invalid_argument("O campo"+std::string(campo.nm_campo, sizeof(campo.nm_campo))+"nao e uma chave valida.");
        }
    }

    bool INSERT(MemoryWrapper<DataBlock> &mem, std::vector<Registro> registros) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        for (int i = 0; i<registros.size(); i++) {

            Campo chave = schema.campos[schema.chave];
            //std::cout << chave.nm_campo << std::endl;
            std::string valor = getValorCampo(chave, &registros[i]);

            // Funcao hash
            size_t bloco = calculateHash(chave, valor, schema.blocos_hash);

            mem.loadBlock(bloco+schema.primeiro_bloco);

            // Operação de Insert
            int insertpos = mem->getPrimeiroRegistroDispEscrita();
            while (insertpos == -1) {
                if (mem->overflow == 0) {
                    schema.ultimo_bloco++;
                    mem->overflow = schema.ultimo_bloco;
                    mem.commitBlock();
                    mem.loadBlock(mem->overflow);
                    mem->initialize();
                    insertpos = 0;
                } else {
                    mem.loadBlock(mem->overflow);
                    insertpos = mem->getPrimeiroRegistroDispEscrita();
                }
            }
            mem->setRegistro(insertpos, registros[i]);
            mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        return true;
    }

    // Um select de comparacao simples pode ser feito usando params do tipo {"CAMPO=valor"}
    // Um select de comparacao em uma faixa pode ser feito usando params do tipo {"CAMPO=[min:max]"}
    // Um select de comparacao em um conjunto de valores pode ser feito usando params do tipo {"CAMPO={valor1,valor2}"}
    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs = std::vector<Registro>();

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        int porChave = -1;

        // Verifica se a chave é um dos alvos
        for (int i = 0; i < targets.size(); i++) {
            if (strncmp(targets[i].campo.nm_campo, schema.campos[schema.chave].nm_campo, sizeof(schema.campos[i].nm_campo)) == 0) {
                porChave = i;
                break;
            }
        }

        if (porChave != -1) {
            switch (targets[porChave].tipo) {
                case VALUE: {
                    // Funcao hash
                    size_t bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[0], schema.blocos_hash);

                    // Busca
                    mem.loadBlock(bloco+schema.primeiro_bloco);
                    while (true) {
                        for (int i = 0; i < schema.regs_por_bloco; i++) {
                            if (mem->isRegistroEscrito(i)) {
                                int match = 0;
                                reg = mem->getRegistro(i);
                                for (int j = 0; j < targets.size(); j++) {
                                    switch (targets[j].tipo) {
                                        case VALUE:
                                            if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0])) match++;
                                            break;
                                        case SET:
                                            for (int v = 0; v < targets[j].valor.size(); v++)
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[v])) {
                                                    match++;
                                                    break;
                                                }
                                            break;
                                        case RANGE:
                                            if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0], ">=") && 
                                                comparaCampo(targets[j].campo, &reg, targets[j].valor[1], "<=")) match++;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                if (match == targets.size()) {
                                    ret_regs.push_back(reg);
                                }
                            }
                        }
                        if (mem->overflow != 0) mem.loadBlock(mem->overflow);
                        else break;
                    }
                    break;
                }
                case SET: {
                    for (int k = 0; k < targets[porChave].valor.size(); k++) {
                        // Funcao hash
                        size_t bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[k], schema.blocos_hash);

                        // Busca
                        mem.loadBlock(bloco+schema.primeiro_bloco);
                        while (true) {
                            for (int i = 0; i < schema.regs_por_bloco; i++) {
                                if (mem->isRegistroEscrito(i)) {
                                    int match = 0;
                                    reg = mem->getRegistro(i);
                                    for (int j = 0; j < targets.size(); j++) {
                                        switch (targets[j].tipo) {
                                            case VALUE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[j].valor.size(); v++)
                                                    if (comparaCampo(targets[j].campo, &reg, targets[j].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0], ">=") && 
                                                    comparaCampo(targets[j].campo, &reg, targets[j].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                    if (match == targets.size()) {
                                        ret_regs.push_back(reg);
                                    }
                                }
                            }
                            if (mem->overflow != 0) mem.loadBlock(mem->overflow);
                            else break;
                        }
                    }
                    break;
                }
                case RANGE: {
                    size_t primeiro_bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[0], schema.blocos_hash);
                    size_t ultimo_bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[1], schema.blocos_hash);
                    
                    for (int k = primeiro_bloco; k < ultimo_bloco; k++) {
                        // Busca
                        mem.loadBlock(k+schema.primeiro_bloco);
                        while (true) {
                            for (int i = 0; i < schema.regs_por_bloco; i++) {
                                if (mem->isRegistroEscrito(i)) {
                                    int match = 0;
                                    reg = mem->getRegistro(i);
                                    for (int j = 0; j < targets.size(); j++) {
                                        switch (targets[j].tipo) {
                                            case VALUE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[j].valor.size(); v++)
                                                    if (comparaCampo(targets[j].campo, &reg, targets[j].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0], ">=") && 
                                                    comparaCampo(targets[j].campo, &reg, targets[j].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                    if (match == targets.size()) {
                                        ret_regs.push_back(reg);
                                    }
                                }
                            }
                            if (mem->overflow != 0) mem.loadBlock(mem->overflow);
                            else break;
                        }
                    }
                    break;
                }
            }

        } else {
            OrganizacaoHeap::SELECT(mem, params);
        }

        return ret_regs;
    }

    // Um select de comparacao simples pode ser feito usando params do tipo {"CAMPO=valor"}
    // Um select de comparacao em uma faixa pode ser feito usando params do tipo {"CAMPO=[min:max]"}
    // Um select de comparacao em um conjunto de valores pode ser feito usando params do tipo {"CAMPO={valor1,valor2}"}
    bool DELETE(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        int porChave = -1;

        // Verifica se a chave é um dos alvos
        for (int i = 0; i < targets.size(); i++) {
            if (strncmp(targets[i].campo.nm_campo, schema.campos[schema.chave].nm_campo, sizeof(schema.campos[i].nm_campo)) == 0) {
                porChave = i;
                break;
            }
        }

        if (porChave != -1) {
            switch (targets[porChave].tipo) {
                case VALUE: {
                    // Funcao hash
                    size_t bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[0], schema.blocos_hash);

                    // Busca
                    mem.loadBlock(bloco+schema.primeiro_bloco);
                    while (true) {
                        bool blockchanged = false;
                        for (int i = 0; i < schema.regs_por_bloco; i++) {
                            if (mem->isRegistroEscrito(i)) {
                                int match = 0;
                                reg = mem->getRegistro(i);
                                for (int j = 0; j < targets.size(); j++) {
                                    switch (targets[j].tipo) {
                                        case VALUE:
                                            if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0])) match++;
                                            break;
                                        case SET:
                                            for (int v = 0; v < targets[j].valor.size(); v++)
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[v])) {
                                                    match++;
                                                    break;
                                                }
                                            break;
                                        case RANGE:
                                            if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0], ">=") && 
                                                comparaCampo(targets[j].campo, &reg, targets[j].valor[1], "<=")) match++;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                if (match == targets.size()) {
                                    mem->deleteRegistro(i);
                                    blockchanged = true;
                                }
                            }
                        }
                        if (mem->overflow != 0) {
                            if (blockchanged) mem.commitBlock();
                            mem.loadBlock(mem->overflow);
                        }
                        else {
                            if (blockchanged) mem.commitBlock();
                            break;
                        }
                    }
                    break;
                }
                case SET: {
                    for (int k = 0; k < targets[porChave].valor.size(); k++) {
                        // Funcao hash
                        size_t bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[k], schema.blocos_hash);

                        // Busca
                        mem.loadBlock(bloco+schema.primeiro_bloco);
                        while (true) {
                            bool blockchanged = false;
                            for (int i = 0; i < schema.regs_por_bloco; i++) {
                                if (mem->isRegistroEscrito(i)) {
                                    int match = 0;
                                    reg = mem->getRegistro(i);
                                    for (int j = 0; j < targets.size(); j++) {
                                        switch (targets[j].tipo) {
                                            case VALUE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[j].valor.size(); v++)
                                                    if (comparaCampo(targets[j].campo, &reg, targets[j].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0], ">=") && 
                                                    comparaCampo(targets[j].campo, &reg, targets[j].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                    if (match == targets.size()) {
                                        mem->deleteRegistro(i);
                                        blockchanged = true;
                                    }
                                }
                            }
                            if (mem->overflow != 0) {
                                if (blockchanged) mem.commitBlock();
                                mem.loadBlock(mem->overflow);
                            }
                            else {
                                if (blockchanged) mem.commitBlock();
                                break;
                            }
                        }
                    }
                    break;
                }
                case RANGE: {
                    size_t primeiro_bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[0], schema.blocos_hash);
                    size_t ultimo_bloco = calculateHash(schema.campos[schema.chave], targets[porChave].valor[1], schema.blocos_hash);
                    
                    for (int k = primeiro_bloco; k < ultimo_bloco; k++) {
                        // Busca
                        mem.loadBlock(k+schema.primeiro_bloco);
                        while (true) {
                            bool blockchanged = false;
                            for (int i = 0; i < schema.regs_por_bloco; i++) {
                                if (mem->isRegistroEscrito(i)) {
                                    int match = 0;
                                    reg = mem->getRegistro(i);
                                    for (int j = 0; j < targets.size(); j++) {
                                        switch (targets[j].tipo) {
                                            case VALUE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[j].valor.size(); v++)
                                                    if (comparaCampo(targets[j].campo, &reg, targets[j].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[j].campo, &reg, targets[j].valor[0], ">=") && 
                                                    comparaCampo(targets[j].campo, &reg, targets[j].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                    if (match == targets.size()) {
                                        mem->deleteRegistro(i);
                                        blockchanged = true;
                                    }
                                }
                            }
                            if (mem->overflow != 0) {
                                if (blockchanged) mem.commitBlock();
                                mem.loadBlock(mem->overflow);
                            }
                            else {
                                if (blockchanged) mem.commitBlock();
                                break;
                            }
                        }
                    }
                    break;
                }
            }

        } else {
            OrganizacaoHeap::SELECT(mem, params);
        }

        return true;
    }

    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        std::vector<Registro> vect;
        /*
        // Teste com um único insert
        vhd.loadBlock(1);
        inserts.push_back(vhd->getRegistro(1));

        INSERT(mem, inserts);

        // Teste com 5 inserts
        inserts.clear();
        vhd.loadBlock(1);
        for (int i = 0; i < 5; i++) {
            inserts.push_back(vhd->getRegistro(i));
        }
        INSERT(mem, inserts);
        */

        // Teste com 10 inserts
        inserts.clear();
        for (int i = 0; i < 2000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);

        //SELECT UM VALOR TEST
        mem.blockAccessCount = 0;
        vect = SELECT(mem, {"NM_CANDIDATO=HELLO WORLD"});
        std::cout << mem.blockAccessCount << std::endl;
        vect = SELECT(mem, {"NM_CANDIDATO=AUGUSTO LUIZ DE LIMA"});
        std::cout << mem.blockAccessCount << std::endl;
        std::cout << mem.blockAccessCount/2 << std::endl;
        mem.blockAccessCount = 0;

        //SELECT MULTIPLOS REGISTROS
        vect = SELECT(mem, {"NM_CANDIDATO={HELLO WORLD,CIRO FERREIRA GOMES}"});
        std::cout << mem.blockAccessCount << vect[0].NM_CANDIDATO <<std::endl;
        mem.blockAccessCount = 0;

        //SELECT FAIXA
        vect = SELECT(mem, {"NM_CANDIDATO=[X:Y]"});
        std::cout << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //SELECT 2 VALORES FIXOS
        vect = SELECT(mem, {"NR_PARTIDO=12","NM_CANDIDATO=CIRO FERREIRA GOMES"});
        std::cout << mem.blockAccessCount << vect[0].NR_PARTIDO << std::endl;
        mem.blockAccessCount = 0;

        cleanup(mem);
    };

}