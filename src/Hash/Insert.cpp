#include <Hash.hpp>

bool Hash::INSERT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<Registro> registros) {

    Schema<Registro> schema;
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