#pragma once
#include <cstring>
#include <string>

enum TipoCampo { INT, BIGINT, CHAR, DATA, HORA, TIMESTAMP, BOOL };

class Campo
{
public:
    int tamanho, pos_relativa;
    char nm_campo[30];
    TipoCampo tipo;

    Campo() {};
    Campo(int t, int pos_rel, const std::string& nome, TipoCampo tp) {
        tamanho = t;
        pos_relativa = pos_rel;
        strncpy(nm_campo, nome.c_str(), sizeof(nm_campo));
        tipo = tp;
    }
    ~Campo() {};
};


// Retorna true se o valor do campo no registro eh ops em relacao ao valor
bool comparaCampo( Campo campo, const void* registro, std::string valor, const std::string &opr = "==");

std::string getValorCampo(Campo campo, const void* registro);