#include <MemoryWrapper.hpp>
#include <Util.hpp>
#include <Registro.hpp>
#include <DataBlock.hpp>
#include <algorithm>


namespace OrganizacaoHash
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

    bool INSERT(MemoryWrapper<DataBlock> &mem, std::vector<Registro> registros);

    std::vector<Registro> SELECT(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params);

    bool DELETE(MemoryWrapper<DataBlock> &mem, std::vector<std::string> params);

    void runTests();
}