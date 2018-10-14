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

namespace OrganizacaoOrdenada
{
    MemoryWrapper<DataBlock> initialize() {
        remove("VHDOrdenado.vhd");
        MemoryWrapper<DataBlock> mem = MemoryWrapper<DataBlock>(vhdf::openDisk("VHDOrdenado.vhd", sizeof(Registro)*40000, true));
        HEAD<Registro> schema = HEAD<Registro>();
        schema.org = HEAP;
        schema.primeiro_bloco = 1;
        schema.ultimo_bloco = 1;
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

    bool INSERT(MemoryWrapper<DataBlock> mem, std::vector<Registro> registros) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        size_t bloco = schema.ultimo_bloco;

        int insertpos;

        for (int i = 0; i<registros.size(); i++) {

            Campo chave = schema.campos[schema.chave];
            //std::cout << chave.nm_campo << std::endl;
            std::string valor = getValorCampo(chave, &registros[i]);

            // Busca binaria
            size_t upper = schema.ultimo_bloco;
            size_t lower = schema.primeiro_bloco;
            size_t middle;

            while (lower < upper) {

                middle = (upper+lower+1)/2;
                mem.loadBlock(middle);

                Registro reg = mem->getRegistro(0);

                if ( comparaCampo(chave, &reg, valor, ">") ) {
                    upper = middle-1;
                }
                else {
                    lower = middle;
                }
            }

            middle = (lower+upper)/2;
            mem.loadBlock(middle);
            // Operação de Insert
            Registro held;
            bool insertOK = false;
            bool needsReorder = false;

            for (int j = 0; j < schema.regs_por_bloco; j++) {
                
                Registro reg = mem->getRegistro(j);
                
                if ( !mem->isRegistroEscrito(j) || comparaCampo(chave, &reg, valor, ">") ) {
                    insertOK = true;
                    insertpos = j;
                    if (mem->isRegistroEscrito(j)) {
                        held = mem->getRegistro(j);
                        needsReorder = true;
                    }
                    mem->setRegistro(j, registros[i]);
                    break;
                }
            }
            if (!insertOK) {
                mem.loadBlock(middle+1);
                if (mem.getLoadedBlockId() > schema.ultimo_bloco) mem->initialize();
                if (mem->isRegistroEscrito(0)) {
                    held = mem->getRegistro(0);
                    needsReorder = true;
                }
                mem->setRegistro(0, registros[i]);
                insertpos = 0;
            }

            // Reordenacao
            if (needsReorder) {
                insertpos++;
                Registro placing;
                for (int j = mem.getLoadedBlockId(); j <= schema.ultimo_bloco+1; j++) {
                    bool stop = false;
                    for (insertpos; insertpos < schema.regs_por_bloco; insertpos++) {
                        std::swap(held, placing);
                        
                        if (mem->isRegistroEscrito(insertpos)) held = mem->getRegistro(insertpos);
                        else stop = true;
                        
                        mem->setRegistro(insertpos, placing);
                        if(stop) break;
                    }
                    mem.commitBlock();
                    if (stop) break;
                    mem.loadBlock(j+1);
                    insertpos = 0;
                    if (mem.getLoadedBlockId() > schema.ultimo_bloco) {
                        mem->initialize();
                        schema.ultimo_bloco = mem.getLoadedBlockId();
                    }
                    bloco++;
                }
            }
            else mem.commitBlock();
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);

        return true;
    }

    // Um select de comparacao simples pode ser feito usando params do tipo {"CAMPO=valor"}
    // Um select de comparacao em uma faixa pode ser feito usando params do tipo {"CAMPO=[min:max]"}
    // Um select de comparacao em um conjunto de valores pode ser feito usando params do tipo {"CAMPO={valor1,valor2}"}
    bool DELETE(MemoryWrapper<DataBlock> mem, std::vector<std::string> params) {
        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);
        std::vector<Registro> ret_regs = std::vector<Registro>();
        std::vector<Target> targets = parseQuery(schema, params);
        Registro reg;
        Campo chave = schema.campos[schema.chave];

        //std::cout << "Comparing against database members..." << std::endl;
        for (int k = 0; k < targets.size(); k++) {
            switch (targets[k].tipo) {
                case VALUE:
                    if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                        size_t upper = schema.ultimo_bloco;
                        size_t lower = schema.primeiro_bloco;
                        size_t middle;
                        std::string valor = targets[k].valor[0];

                        while (lower < upper) {
                            middle = (upper+lower+1)/2;
                            mem.loadBlock(middle);
                            Registro reg = mem->getRegistro(0);
                            if ( comparaCampo(chave, &reg, valor, ">") ) {
                                upper = middle-1;
                            }
                            else {
                                lower = middle;
                            }
                        }
                        mem.loadBlock((lower+upper)/2);
                        for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                            reg = mem->getRegistro(j);
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0])){
                                int match = 0;
                                for(int i = 0; i < targets.size(); i++){
                                    switch (targets[i].tipo){
                                        case VALUE:
                                            if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0])) match++;
                                            break;
                                        case SET:
                                            for (int v = 0; v < targets[i].valor.size(); v++)
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[v])) {
                                                    match++;
                                                    break;
                                                }
                                            break;
                                        case RANGE:
                                            if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0], ">=") &&
                                                comparaCampo(targets[i].campo, &reg, targets[i].valor[1], "<=")) match++;
                                            break;
                                        default:
                                            break;
                                    }
                                }
                                if(match==targets.size()){
                                    if(j == 0){
                                        if(mem->registrosEscritos.size() > 1){
                                            Registro regAux;
                                            regAux = mem->getRegistro(1);
                                            mem->deleteRegistro(0);
                                            mem->deleteRegistro(1);
                                            mem->setRegistro(0, regAux);
                                        }
                                    }
                                    else{
                                        mem->deleteRegistro(j);
                                    }
                                    mem.commitBlock();
                                }
                            }
                        }
                    }
                    else{

                    }
                    break;
                case SET:
                    for (int v = 0; v < targets[k].valor.size(); v++){
                        if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                            size_t upper = schema.ultimo_bloco;
                            size_t lower = schema.primeiro_bloco;
                            size_t middle;
                            std::string valor = targets[k].valor[v];
                            while (lower < upper) {
                                middle = (upper+lower+1)/2;
                                mem.loadBlock(middle);
                                Registro reg = mem->getRegistro(0);
                                if ( comparaCampo(chave, &reg, valor, ">") ) {
                                    upper = middle-1;
                                }
                                else {
                                    lower = middle;
                                }
                            }
                            mem.loadBlock((lower+upper)/2);
                            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                                reg = mem->getRegistro(j);
                                if (comparaCampo(targets[k].campo, &reg, targets[k].valor[v])){
                                    int match = 0;
                                    for(int i = 0; i < targets.size(); i++){
                                        switch (targets[i].tipo){
                                            case VALUE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[i].valor.size(); v++)
                                                    if (comparaCampo(targets[i].campo, &reg, targets[i].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0], ">=") &&
                                                    comparaCampo(targets[i].campo, &reg, targets[i].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }

                                    }
                                    if(match==targets.size()) {
                                        if (j == 0) {
                                            if (mem->registrosEscritos.size() > 1) {
                                                Registro regAux;
                                                regAux = mem->getRegistro(1);
                                                mem->deleteRegistro(0);
                                                mem->deleteRegistro(1);
                                                mem->setRegistro(0, regAux);
                                            }
                                        } else {
                                            mem->deleteRegistro(j);
                                        }
                                        mem.commitBlock();
                                    }
                                }
                            }
                        }
                    }
                    break;
                case RANGE:
                    if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                        size_t upper = schema.ultimo_bloco;
                        size_t lower = schema.primeiro_bloco;
                        size_t middle;
                        std::string valor = targets[k].valor[0];

                        while (lower < upper) {
                            middle = (upper+lower+1)/2;
                            mem.loadBlock(middle);
                            Registro reg = mem->getRegistro(0);
                            if ( comparaCampo(chave, &reg, valor, ">") ) {
                                upper = middle-1;
                            }
                            else {
                                lower = middle;
                            }
                        }
                        bool inrange = true;
                        int w = 0;
                        while(inrange) {
                            mem.loadBlock(w + (lower + upper) / 2);
                            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                                reg = mem->getRegistro(j);
                                if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0], ">=") &&
                                    comparaCampo(targets[k].campo, &reg, targets[k].valor[1], "<=")) {
                                    int match = 0;
                                    for(int i = 0; i < targets.size(); i++){
                                        switch (targets[i].tipo){
                                            case VALUE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[i].valor.size(); v++)
                                                    if (comparaCampo(targets[i].campo, &reg, targets[i].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0], ">=") &&
                                                    comparaCampo(targets[i].campo, &reg, targets[i].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }

                                    }
                                    if(match==targets.size())
                                        ret_regs.push_back(reg);
                                }else if(comparaCampo(targets[k].campo, &reg, targets[k].valor[1], ">"))
                                    inrange=false;
                            }
                            w++;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        return true;
    }


    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> mem, std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs = std::vector<Registro>();

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        Campo chave = schema.campos[schema.chave];

        //std::cout << "Comparing against database members..." << std::endl;
        for (int k = 0; k < targets.size(); k++) {
            switch (targets[k].tipo) {
                case VALUE:
                    if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                        size_t upper = schema.ultimo_bloco;
                        size_t lower = schema.primeiro_bloco;
                        size_t middle;
                        std::string valor = targets[k].valor[0];

                        while (lower < upper) {
                            middle = (upper+lower+1)/2;
                            mem.loadBlock(middle);
                            Registro reg = mem->getRegistro(0);
                            if ( comparaCampo(chave, &reg, valor, ">") ) {
                                upper = middle-1;
                            }
                            else {
                                lower = middle;
                            }
                        }
                        mem.loadBlock((lower+upper)/2);
                        for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                            reg = mem->getRegistro(j);
                            if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0])){
                                int match = 0;
                                for(int i = 0; i < targets.size(); i++){
                                    switch (targets[i].tipo){
                                        case VALUE:
                                            if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0])) match++;
                                            break;
                                        case SET:
                                            for (int v = 0; v < targets[i].valor.size(); v++)
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[v])) {
                                                    match++;
                                                    break;
                                                }
                                            break;
                                        case RANGE:
                                            if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0], ">=") &&
                                                comparaCampo(targets[i].campo, &reg, targets[i].valor[1], "<=")) match++;
                                            break;
                                        default:
                                            break;
                                    }

                                }
                                if(match==targets.size())
                                    ret_regs.push_back(reg);
                            }
                        }
                    }
                    else{

                    }
                    break;
                case SET:
                    for (int v = 0; v < targets[k].valor.size(); v++){
                        if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                            size_t upper = schema.ultimo_bloco;
                            size_t lower = schema.primeiro_bloco;
                            size_t middle;
                            std::string valor = targets[k].valor[v];
                            while (lower < upper) {
                                middle = (upper+lower+1)/2;
                                mem.loadBlock(middle);
                                Registro reg = mem->getRegistro(0);
                                if ( comparaCampo(chave, &reg, valor, ">") ) {
                                    upper = middle-1;
                                }
                                else {
                                    lower = middle;
                                }
                            }
                            mem.loadBlock((lower+upper)/2);
                            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                                reg = mem->getRegistro(j);
                                if (comparaCampo(targets[k].campo, &reg, targets[k].valor[v])){
                                    int match = 0;
                                    for(int i = 0; i < targets.size(); i++){
                                        switch (targets[i].tipo){
                                            case VALUE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[i].valor.size(); v++)
                                                    if (comparaCampo(targets[i].campo, &reg, targets[i].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0], ">=") &&
                                                    comparaCampo(targets[i].campo, &reg, targets[i].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }

                                    }
                                    if(match==targets.size())
                                        ret_regs.push_back(reg);
                                }
                            }
                        }
                    }
                    break;
                case RANGE:
                    if(strcmp(targets[k].campo.nm_campo, chave.nm_campo) == 0){
                        size_t upper = schema.ultimo_bloco;
                        size_t lower = schema.primeiro_bloco;
                        size_t middle;
                        std::string valor = targets[k].valor[0];

                        while (lower < upper) {
                            middle = (upper+lower+1)/2;
                            mem.loadBlock(middle);
                            Registro reg = mem->getRegistro(0);
                            if ( comparaCampo(chave, &reg, valor, ">") ) {
                                upper = middle-1;
                            }
                            else {
                                lower = middle;
                            }
                        }
                        bool inrange = true;
                        int w = 0;
                        while(inrange) {
                            mem.loadBlock(w + (lower + upper) / 2);
                            for (int j = 0; j < mem->registrosEscritos.size(); j++) {
                                reg = mem->getRegistro(j);
                                if (comparaCampo(targets[k].campo, &reg, targets[k].valor[0], ">=") &&
                                    comparaCampo(targets[k].campo, &reg, targets[k].valor[1], "<=")) {
                                    int match = 0;
                                    for(int i = 0; i < targets.size(); i++){
                                        switch (targets[i].tipo){
                                            case VALUE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0])) match++;
                                                break;
                                            case SET:
                                                for (int v = 0; v < targets[i].valor.size(); v++)
                                                    if (comparaCampo(targets[i].campo, &reg, targets[i].valor[v])) {
                                                        match++;
                                                        break;
                                                    }
                                                break;
                                            case RANGE:
                                                if (comparaCampo(targets[i].campo, &reg, targets[i].valor[0], ">=") &&
                                                    comparaCampo(targets[i].campo, &reg, targets[i].valor[1], "<=")) match++;
                                                break;
                                            default:
                                                break;
                                        }

                                    }
                                    if(match==targets.size())
                                        ret_regs.push_back(reg);
                                }else if(comparaCampo(targets[k].campo, &reg, targets[k].valor[1], ">"))
                                    inrange=false;
                            }
                            w++;
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        return ret_regs;
    }



    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        std::vector<Registro> vect;

//        // Teste com um único insert
//        vhd.loadBlock(1);
//        inserts.push_back(vhd->getRegistro(1));
//
//        INSERT(mem, inserts);

//        // Teste com 5 inserts
//        inserts.clear();
//        vhd.loadBlock(1);
//        for (int i = 0; i < 5; i++) {
//            inserts.push_back(vhd->getRegistro(i));
//        }
//        INSERT(mem, inserts);

        // Teste com 1000 inserts
        inserts.clear();
        for (int i = 0; i < 1000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);
        //mem.blockAccessCount = 0;

        //Teste Select
        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});
        std::cout << mem.blockAccessCount << std::endl << vect[0].NM_CANDIDATO << std::endl;

        DELETE(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});

        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});
        std::cout << mem.blockAccessCount << std::endl << vect[0].NM_CANDIDATO << std::endl;

//        //Teste Select Range
//        vect = SELECT(mem, {"NM_CANDIDATO=[MARIA DO CARMO OLIVEIRA NAFALSKI:MARIA DO SOCORRO NASCIMENTO BARBOSA]","ANO_ELEICAO=[2015:2018]"});
//        for(int i=0; i < vect.size(); i++){
//           std::cout << vect[i].NM_CANDIDATO << std::endl;
//        }

        //Teste Multiplos Registros
//        vect = SELECT(mem, {"NM_CANDIDATO={MARIA DO CARMO OLIVEIRA NAFALSKI,MARIA DO SOCORRO NASCIMENTO BARBOSA}","ANO_ELEICAO=[2015:2020]"});
//        for(int i=0; i < vect.size(); i++){
//            std::cout << vect[i].NM_CANDIDATO << std::endl;
//        }

        vhdf::closeDisk(vhd.getDiskId());

        cleanup(mem);
    };

}