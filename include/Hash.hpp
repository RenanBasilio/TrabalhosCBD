#pragma once
#include <algorithm>
#include <cmath>

#include <Campo.hpp>
#include <DataBlock.hpp>
#include <MemoryWrapper.hpp>
#include <Heap.hpp>
#include <Query.hpp>
#include <Registro.hpp>
#include <Schema.hpp>
#include <Util.hpp>

namespace Hash
{
    template <typename T>
    inline size_t hashingFunc(size_t tablewidth, T value) {
        return (value % tablewidth);
    };

    template <>
    inline size_t hashingFunc<std::string>(size_t tablewidth, std::string value) {
        size_t result = 0;
        size_t chars_signif = std::min ( std::ceil(tablewidth / (float)0xff) , (float)value.length() );
        for (int i = 0; i < chars_signif; i++) {
            result += value[i] % tablewidth;
        }
        return result;
    };

    size_t calculateHash(Campo campo, std::string chave, size_t tamanho);

    bool INSERT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<Registro> registros);

    std::vector<Registro> SELECT(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params);

    bool DELETE(MemoryWrapper<DataBlock<Registro>> &mem, std::vector<std::string> params);

    void runTests();
}