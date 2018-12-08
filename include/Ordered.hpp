#pragma once
#include <iostream>

#include <MemoryWrapper.hpp>
#include <Util.hpp>
#include <DataBlock.hpp>
#include <Schema.hpp>
#include <Campo.hpp>

namespace Ordered
{
    //void runTests();

    //std::vector<Registro> SELECT(MemoryWrapper<DataBlock<Registro>> mem, std::vector<std::string> params);

    //bool DELETE(MemoryWrapper<DataBlock<Registro>> mem, std::vector<std::string> params);

    bool INSERT(MemoryWrapper &mem, const std::vector<void*> &registros);

}