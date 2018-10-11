#pragma once

class Campo
{
private:
    int tamanho, pos_relativa;
    char[30] nm_campo;
public:
    Campo() {};
    Campo(int t, int pos_rel, char* nome) {
        tamanho = t;
        pos_relativa = pos_rel;
        nm_campo = nome;
    }
    ~Campo() {};
}