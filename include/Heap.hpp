#pragma once

#include <Campo.hpp>
#include <DataBlock.hpp>
#include <MemoryWrapper.hpp>
#include <Registro.hpp>
#include <Schema.hpp>
#include <Util.hpp>
#include <Query.hpp>

namespace Heap
{
    void runTests();

    bool INSERT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<Registro> registros);

    std::vector<Registro> SELECT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params);

    bool DELETE(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params);
}