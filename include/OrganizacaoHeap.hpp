
namespace OrganizacaoHeap
{
    void runTests();

    bool INSERT(std::vector<Registro> registros);

    std::vector<Registro> SELECT(size_t id, int campos...);
}