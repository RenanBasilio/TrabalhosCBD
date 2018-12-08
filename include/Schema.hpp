#pragma once
#include <vector>

#include <Serializable.hpp>
#include <Campo.hpp>
#include <Timestamp.hpp>

enum Organizacao { HEAP, ORDERED, HASH, BTREE };

class Schema : public Serializable
{
public:
    size_t primeiro_bloco = 0, ultimo_bloco = 0, blocos_hash = 0;
    unsigned int tamanho = 0, regs_por_bloco = 0, offset = 0, chave = 0;
    Timestamp criacao, alteracao;
    Organizacao org;
    unsigned int nCampos = 0;
    std::vector<Campo> campos;

    Schema() { 
        campos = std::vector<Campo>();
    }

    Schema(const std::vector<Campo>& _campos) {
        campos = std::vector<Campo>(_campos);
        nCampos = campos.size();
        for( auto c : campos) {
            tamanho += c.tamanho;
        }
        regs_por_bloco = vhdf::BLOCK_SIZE/tamanho;
    }

    virtual size_t serialize_size() const {
        return sizeof(primeiro_bloco) + sizeof(ultimo_bloco) + 
                sizeof(blocos_hash) + sizeof(tamanho) + 
                sizeof(regs_por_bloco) + sizeof(offset) + 
                sizeof(chave) + sizeof(criacao) + sizeof(alteracao) +
                sizeof(org) + sizeof(nCampos);
    }

    virtual void serialize( char* data ) const { 
        size_t cont_size = serialize_size();
        memcpy(data, &primeiro_bloco, cont_size);
        for (size_t i = 0; i < nCampos; i++) {
            char* pos = data + cont_size + (i*sizeof(Campo));
            campos[i].serialize(pos);
        }
    }

    virtual void deserialize( const char* data ) {
        size_t cont_size = serialize_size();
        memcpy(&primeiro_bloco, data, cont_size);
        for (size_t i = 0; i < nCampos; i++) {
            Campo cp;
            const char* pos = data + cont_size + (i*sizeof(Campo));
            cp.deserialize(pos);
            campos.push_back(cp);
        }
    }

    std::string toString() {
        std::string str = "Schema[\n  start=" + std::to_string(primeiro_bloco) 
                        + ", end=" + std::to_string(ultimo_bloco) 
                        + ", size=" + std::to_string(tamanho) + " ( " + std::to_string(regs_por_bloco) + " /block )"
                        + ", fields=" + std::to_string(nCampos)
                        + "\n";
        for (size_t i = 0; i < campos.size(); i++) {
            str += "  " + campos[i].toString() + "\n";
        }
        str += "]";
        return str;
    }
};