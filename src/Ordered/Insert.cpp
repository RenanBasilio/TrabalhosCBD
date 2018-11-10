#include <Ordered.hpp>

bool Ordered::INSERT(MemoryWrapper<DataBlock<Registro>> mem, std::vector<Registro> registros) {
    int count = 0;
    Schema<Registro> schema;
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
