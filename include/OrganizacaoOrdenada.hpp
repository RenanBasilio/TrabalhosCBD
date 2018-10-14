#include <MemoryWrapper.hpp>
#include <Util.hpp>
#include <Registro.hpp>
#include <DataBlock.hpp>

namespace OrganizacaoOrdenada
{
    void runTests();

    bool INSERT(MemoryWrapper<DataBlock> mem, std::vector<Registro> registros);
}