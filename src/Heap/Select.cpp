#include <Heap.hpp>

// Um select de comparacao simples pode ser feito usando params do tipo {"CAMPO=valor"}
// Um select de comparacao em uma faixa pode ser feito usando params do tipo {"CAMPO=[min:max]"}
// Um select de comparacao em um conjunto de valores pode ser feito usando params do tipo {"CAMPO={valor1,valor2}"}
std::vector<Registro> Heap::SELECT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params) {

    Schema<Registro> schema;
    vhdf::readBlock(mem.getDiskId(), 0, &schema);

    std::vector<Registro> ret_regs = std::vector<Registro>();

    std::vector<Target> targets = parseQuery(schema, params);

    Registro reg;
    
    //std::cout << "Comparing against database members..." << std::endl;
    for (size_t i = schema.primeiro_bloco; i <= schema.ultimo_bloco; i++) {
        mem.loadBlock(i);
        for (int j = 0; j < mem->registrosEscritos.size(); j++) {
            reg = mem->getRegistro(j);

            int match = 0;
            for (int k = 0; k < targets.size(); k++) matchQuery(targets[k], &reg) ? match++ : match;
            if (match == targets.size()) {
                ret_regs.push_back(reg);
                //std::cout << "Found match " << reg.NM_CANDIDATO << std::endl;
            }
        }
    }

    return ret_regs;
}
