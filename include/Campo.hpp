#pragma once
#include <cstring>
#include <string>

enum tipoCampo { INT, BIGINT, CHAR, DATA, HORA, TIMESTAMP };

class Campo
{
private:
    char nm_campo[30];
    tipoCampo tipo;
public:
    int tamanho, pos_relativa;

    Campo() {};
    Campo(int t, int pos_rel, const std::string& nome) {
        tamanho = t;
        pos_relativa = pos_rel;
        strncpy(nm_campo, nome.c_str(), sizeof(nm_campo));
    }
    ~Campo() {};
};