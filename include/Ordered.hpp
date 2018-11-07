#pragma once
#include <iostream>

#include <MemoryWrapper.hpp>
#include <Util.hpp>
#include <Registro.hpp>
#include <DataBlock.hpp>
#include <Query.hpp>
#include <Schema.hpp>
#include <Campo.hpp>

namespace Ordered
{
    void runTests();

    bool INSERT(MemoryWrapper<DataBlock> mem, std::vector<Registro> registros);

    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> mem, std::vector<std::string> params);

    bool DELETE(MemoryWrapper<DataBlock> mem, std::vector<std::string> params);

}