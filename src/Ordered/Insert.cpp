#include <Ordered.hpp>

bool Ordered::INSERT(MemoryWrapper &mem, const std::vector<void*> &registros) {
    int count = 0;
    Schema schema = mem.getSchema();

    int insertpos;

    void* reg = malloc(schema.tamanho);

    for (int i = 0; i<registros.size(); i++) {

        Campo chave = schema.campos[schema.chave];
        //std::cout << chave.nm_campo << std::endl;
        std::string valor = getValorCampo(chave, registros.at(i));

        // Busca binaria
        size_t upper = schema.ultimo_bloco;
        size_t lower = schema.primeiro_bloco;
        size_t middle;

        while (lower < upper) {

            middle = (upper+lower+1)/2;
            mem.loadBlock(middle);
            count++;
            
            mem->getRegistro(0, reg);

            if ( comparaCampo(chave, reg, valor, ">") ) {
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
        void* held = malloc(schema.tamanho);
        bool insertOK = false;
        bool needsReorder = false;

        for (int j = 0; j < schema.regs_por_bloco; j++) {
            
            mem->getRegistro(j, reg);
            
            if ( !mem->isRegistroEscrito(j) || comparaCampo(chave, reg, valor, ">") ) {
                insertOK = true;
                insertpos = j;
                if (mem->isRegistroEscrito(j)) {
                    mem->getRegistro(j, held);
                    needsReorder = true;
                }
                mem->setRegistro(j, registros.at(i));
                break;
            }
        }
        if (!insertOK) {
            mem.loadBlock(middle+1);
            count++;
            if (mem.getLoadedBlockId() > schema.ultimo_bloco) {
                schema.ultimo_bloco = mem.getLoadedBlockId();
                mem.updateSchema(schema);
            }
            if (mem->isRegistroEscrito(0)) {
                mem->getRegistro(0, held);
                needsReorder = true;
            }
            mem->setRegistro(0, registros[i]);
            insertpos = 0;
        }

        // Reordenacao
        if (needsReorder) {
            insertpos++;
            for (int j = mem.getLoadedBlockId(); j <= schema.ultimo_bloco+1; j++) {
                bool stop = false;
                for (insertpos; insertpos < schema.regs_por_bloco; insertpos++) {
                    std::swap(held, reg);
                    
                    if (mem->isRegistroEscrito(insertpos)) mem->getRegistro(insertpos, held);
                    else stop = true;
                    
                    mem->setRegistro(insertpos, reg);
                    if(stop) break;
                }
                mem.commitBlock();
                count++;
                if (stop) break;
                mem.loadBlock(j+1);
                count++;
                insertpos = 0;
                if (mem.getLoadedBlockId() > schema.ultimo_bloco) {
                    schema.ultimo_bloco = mem.getLoadedBlockId();
                    mem.updateSchema(schema);
                }
            }
        }
        else{ 
            mem.commitBlock(); 
            count++;
        }
    }

    mem.updateSchema(schema);
    //std::cout << count << std::endl;
    return true;
};
