#include <Ordered.hpp>

std::vector<Registro> Ordered::SELECT(MemoryWrapper<DataBlock> mem, std::vector<std::string> params) {

    Schema<Registro> schema;
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
                            for(int i = 0; i < targets.size(); i++) matchQuery(targets[k], &reg) ? match++ : match;
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
                                for(int i = 0; i < targets.size(); i++) matchQuery(targets[k], &reg) ? match++ : match;
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
                                for(int i = 0; i < targets.size(); i++) matchQuery(targets[k], &reg) ? match++ : match;
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
