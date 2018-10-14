#include <MemoryWrapper.hpp>
#include <Util.hpp>
#include <Registro.hpp>
#include <DataBlock.hpp>

namespace OrganizacaoHeap
{
    void runTests();

    bool INSERT(MemoryWrapper<DataBlock> &mem, std::vector<Registro> registros);

    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params);

    bool DELETE(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params);
}