
namespace OrganizacaoHash
{
    template <typename T>
    inline size_t hashingFunc(size_t tablewidth, T value) {
        return (value % tablewidth);
    };

    template <>
    inline size_t hashingFunc<std::string>(size_t tablewidth, std::string value) {
        size_t result = 0;
        size_t chars_signif = std::ceil(tablewidth / (float)0xff);
        for (int i = 0; i < chars_signif; i++) {
            result += value[i] % tablewidth;
        }
        return result;
    };

    void runTests();
}