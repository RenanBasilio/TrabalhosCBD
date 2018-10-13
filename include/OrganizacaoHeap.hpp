
namespace OrganizacaoHeap
{
    void runTests();

    bool INSERT(std::vector<Registro> registros);

    std::vector<Registro> SELECT(std::vector<std::string> params);

    bool DELETE(std::vector<std::string> params);
}