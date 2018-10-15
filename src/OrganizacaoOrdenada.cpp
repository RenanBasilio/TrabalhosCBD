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
        schema.chave = 9;   // SQ_CANDIDATO
        //schema.chave = 11; // NM_CANDIDATO
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
        int count = 0;
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
                count++;

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
            count++;
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
                count++;
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
                    count++;
                    if (stop) break;
                    mem.loadBlock(j+1);
                    count++;
                    insertpos = 0;
                    if (mem.getLoadedBlockId() > schema.ultimo_bloco) {
                        mem->initialize();
                        schema.ultimo_bloco = mem.getLoadedBlockId();
                    }
                    bloco++;
                }
            }
            else{ mem.commitBlock(); count++;
            }
        }

        vhdf::writeBlock(mem.getDiskId(), 0, &schema);
        std::cout << count << std::endl;
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
        int count = 0;

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
                            count++;
                            Registro reg = mem->getRegistro(0);
                            if ( comparaCampo(chave, &reg, valor, ">") ) {
                                upper = middle-1;
                            }
                            else {
                                lower = middle;
                            }
                        }
                        mem.loadBlock((lower+upper)/2);
                        count++;
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
                                    count++;
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
                                count++;
                                Registro reg = mem->getRegistro(0);
                                if ( comparaCampo(chave, &reg, valor, ">") ) {
                                    upper = middle-1;
                                }
                                else {
                                    lower = middle;
                                }
                            }
                            mem.loadBlock((lower+upper)/2);
                            count++;
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
                                        count++;
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
                            count++;
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
                            count++;
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

        std::cout << count << std::endl;
        return true;
    }


    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> mem, std::vector<std::string> params) {

        HEAD<Registro> schema;
        vhdf::readBlock(mem.getDiskId(), 0, &schema);

        std::vector<Registro> ret_regs = std::vector<Registro>();

        std::vector<Target> targets = parseQuery(schema, params);

        Registro reg;
        Campo chave = schema.campos[schema.chave];
        int count = 0;

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
                            count++;
                            Registro reg = mem->getRegistro(0);
                            if ( comparaCampo(chave, &reg, valor, ">") ) {
                                upper = middle-1;
                            }
                            else {
                                lower = middle;
                            }
                        }
                        mem.loadBlock((lower+upper)/2);
                        count++;
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
                                count++;
                                Registro reg = mem->getRegistro(0);
                                if ( comparaCampo(chave, &reg, valor, ">") ) {
                                    upper = middle-1;
                                }
                                else {
                                    lower = middle;
                                }
                            }
                            mem.loadBlock((lower+upper)/2);
                            count++;
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
                            count++;
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
                            count++;
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

        std::cout << count << std::endl;
        return ret_regs;
    }



    void runTests() {
        MemoryWrapper<DataBlock> mem = initialize();

        std::vector<Registro> inserts = std::vector<Registro>();
        MemoryWrapper<DataBlock> vhd(vhdf::openDisk("testdisk.vhd"));
        std::vector<Registro> vect;

        // Teste com 1000 inserts
        inserts.clear();
        for (int i = 0; i < 2000; i++) {
            vhd.loadBlock(1+i);
            for (int j = 0; j < 10; j++) {
                inserts.push_back(vhd->getRegistro(j));
            }
        }
        INSERT(mem, inserts);


        //INSERT UNICO TEST
        mem.blockAccessCount = 0;
        vhd.loadBlock(1);
        inserts.push_back(vhd->getRegistro(1));
        INSERT(mem, inserts);
        std::cout << "INSERT 1 registro: " << mem.blockAccessCount << std::endl;

        // Teste com 10 inserts
        mem.blockAccessCount = 0;
        inserts.clear();
        vhd.loadBlock(1);
        for (int i = 0; i < 10; i++) {
            inserts.push_back(vhd->getRegistro(i));
        }
        INSERT(mem, inserts);
        std::cout << "INSERT 10 Registros: " << mem.blockAccessCount << std::endl;

           //SELECT UM VALOR TEST (SQ_CANDIDATO)
        mem.blockAccessCount = 0;
        vect = SELECT(mem, {"SQ_CANDIDATO=280000605589"});
        //std::cout << mem.blockAccessCount << std::endl;
        vect = SELECT(mem, {"SQ_CANDIDATO=80000624525"});
        //std::cout << mem.blockAccessCount << std::endl;
        std::cout << "Média (SELECT SQ_CANDIDATO=280000605589 E SELECT SQ_CANDIDATO=80000624525): " << mem.blockAccessCount/2 << std::endl;
        mem.blockAccessCount = 0;

        //SELECT MULTIPLOS REGISTROS SQ_CANDIDATO
        vect = SELECT(mem, {"SQ_CANDIDATO={280000605589,80000624525}"});
        std::cout << "SELECT SQ_CANDIDATO={280000605589,80000624525} " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;

        //SELECT FAIXA SQ_CANDIDATO
        vect = SELECT(mem, {"SQ_CANDIDATO=[280000605589:80000624525]"});
        std::cout << "SELECT SQ_CANDIDATO=[280000605589:80000624525] " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;
    
        //SELECT 2 VALORES FIXOS
        vect = SELECT(mem, {"NR_PARTIDO=12","SQ_CANDIDATO=280000605589"});
        std::cout << "SELECT NR_PARTIDO=12,SQ_CANDIDATO=280000605589 : " << mem.blockAccessCount <<std::endl;
        mem.blockAccessCount = 0;

        //DELETE UM REG
        DELETE(mem, {"SQ_CANDIDATO=280000605589"});
        std::cout << "DELETE SQ_CANDIDATO=280000605589: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE MULTIPLOS REG
        DELETE(mem, {"SQ_CANDIDATO={280000605589,80000624525}"});
        std::cout << "DELETE SQ_CANDIDATO={280000605589,80000624525}: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE FAIXA
        DELETE(mem, {"NR_PARTIDO=[10:12]"});
        std::cout << "DELETE NR_PARTIDO=[10:12]: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;

        //DELETE 2 VALORES FIXOS
        DELETE(mem, {"NR_PARTIDO=12","SQ_CANDIDATO=280000605589"});
        std::cout << "DELETE NR_PARTIDO=12,SQ_CANDIDATO=280000605589: " << mem.blockAccessCount << std::endl;
        mem.blockAccessCount = 0;



        /*

        //INSERT UNICO TEST
        Registro reg;
        strncpy(reg.NM_CANDIDATO, "HELLO WORLD", sizeof(reg.NM_CANDIDATO));
        INSERT(mem, {reg});

        //INSERT DUPLO TEST
        std::vector<Registro> reglist(2);
        strncpy(reglist[0].NM_CANDIDATO, "ROBSON", sizeof(reg.NM_CANDIDATO));
        strncpy(reglist[1].NM_CANDIDATO, "ANDERSON", sizeof(reg.NM_CANDIDATO));
        INSERT(mem, reglist);

        //SELECT UM VALOR TEST
        vect = SELECT(mem, {"NM_CANDIDATO=HELLO WORLD"});

        //SELECT MULTIPLOS REGISTROS
        vect = SELECT(mem, {"NM_CANDIDATO={HELLO WORLD,MARIA DO SOCORRO NASCIMENTO BARBOSA}"});


        //SELECT FAIXA
        vect = SELECT(mem, {"NM_CANDIDATO=[MARIA DO CARMO OLIVEIRA NAFALSKI:MARIA DO SOCORRO NASCIMENTO BARBOSA]"});

        //SELECT 2 VALORES FIXOS
        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});

        //DELETE UM REG
        DELETE(mem, {"NM_CANDIDATO=HELLO WORLD"});

        //DELETE MULTIPLOS REG
        DELETE(mem, {"NM_CANDIDATO={robson,MARIA DO CARMO OLIVEIRA NAFALSKI}"});

       //DELETE FAIXA
       DELETE(mem, {"NR_PARTIDO=[10:12]"});
       //std::cout << mem.blockAccessCount << std::endl;
       //mem.blockAccessCount = 0;

        //DELETE 2 VALORES FIXOS
        DELETE(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});

        */


//        //Teste Select
//        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA","ANO_ELEICAO=[2015:2020]","SG_UE=MA"});
//        std::cout << mem.blockAccessCount << std::endl << vect[0].NM_CANDIDATO << std::endl;
//
//        DELETE(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA,MARIA DO CARMO OLIVEIRA NAFALSKI","ANO_ELEICAO=[2015:2020]"});
//
//        vect = SELECT(mem, {"NM_CANDIDATO=MARIA DO SOCORRO NASCIMENTO BARBOSA,MARIA DO CARMO OLIVEIRA NAFALSKI"});
//        std::cout << mem.blockAccessCount << std::endl << vect[0].NM_CANDIDATO << std::endl;
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