#pragma once
#include <cstring>

enum tipoCampo { INT, BIGINT, CHAR, DATA, HORA, TIMESTAMP };

class Campo
{
private:
    char nm_campo[30];
    tipoCampo tipo;
public:
    int tamanho, pos_relativa;

    Campo() {};
    Campo(int t, int pos_rel, char* nome) {
        tamanho = t;
        pos_relativa = pos_rel;
        strncpy(nm_campo, nome, sizeof(nm_campo));
    }
    ~Campo() {};
};