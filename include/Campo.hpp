#pragma once
#include <string>
#include <Serializable.hpp>

enum TipoCampo { INT, BIGINT, CHAR, DATA, HORA, TIMESTAMP, BOOL };

class Campo : public Serializable
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

    virtual size_t serialize_size() const {
        return sizeof(tamanho) + sizeof(pos_relativa) + sizeof(nm_campo) + sizeof(tipo);
    }

    virtual void serialize( char* data ) const {
        memcpy(data, &tamanho, serialize_size());
    }

    virtual void deserialize( const char* data ) {
        memcpy(&tamanho, data, serialize_size());
    }

    std::string toString() {
        std::string str = "Campo[ " + std::string(nm_campo, sizeof(nm_campo)) + ", tamanho=" + std::to_string(tamanho) + ", offset=" + std::to_string(pos_relativa) + " ]";
        return str;
    }
};


// Retorna true se o valor do campo no registro eh ops em relacao ao valor
bool comparaCampo( Campo campo, const void* registro, std::string valor, const std::string &opr = "==");

std::string getValorCampo(Campo campo, const void* registro);