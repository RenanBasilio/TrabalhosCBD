#include <vector>
#include <algorithm>
#include <unordered_map>

#include <vhdf.hpp>
#include <MemoryWrapper.hpp>
#include <DataBlock.hpp>
#include <Campo.hpp>
#include <Schema.hpp>

// Essa classe representa um indice de uma tabela em memoria
class Index {
    // createIndex e um friend de Index. Isso da acesso ao construtor privado aa funcao createIndex
    template <typename R>
    friend Index createIndex( MemoryWrapper<DataBlock<R>> mem, Campo c, Organizacao org );

    // A estrutura de dados utilizada pelo indice na organizacao org
    void* _structure = nullptr;
    Organizacao _org;

private:
    // Esse construtor nao pode ser chamado diretamente. Utilize a funcao createIndex
    Index( Organizacao org, std::vector<std::pair<std::string, size_t>> vals ) {
        _org = org;
        sort( vals );
    }

    // Organiza as entradas de vals conforme a organizacao desse indice
    void sort( std::vector<std::pair<std::string, size_t>> &vals ) {
        switch (_org)
        {
            case ORDERED: {
                std::sort( vals.begin(), vals.end() );
                _structure = new std::vector<std::pair<std::string, size_t>>(vals);
            }
            break;
            case HASH: {
                std::unordered_map<std::string, std::vector<size_t>> *hashtable = new std::unordered_map<std::string, std::vector<size_t>>();
                for ( auto var : vals) {
                    if ( hashtable->find(var.first) == hashtable->end() ) hashtable->insert( {var.first, std::vector<size_t>()} );
                    hashtable->at(var.first).push_back(var.second);
                } 
                _structure = hashtable;
            }
            break;
            case BTREE:
                break;
            default:
                break;
        }
    }

public:
    // Recupera a lista de blocos com a chave key
    std::vector<size_t> findBlocks ( std::string key ) {
        std::vector<size_t> blocks = std::vector<size_t>();
        switch (_org)
        {
            case ORDERED: {
                std::vector<std::pair<std::string, size_t>> *vector = static_cast<std::vector<std::pair<std::string, size_t>>*>(_structure);
                // TODO: busca binaria
                for (size_t i = 0; i < vector->size(); i++) {
                    if( vector->at(i).first == key ) blocks.push_back(vector->at(i).second);
                }
                return blocks;
            }
            case HASH:{
                std::unordered_map<std::string, std::vector<size_t>> *hashtable = static_cast<std::unordered_map<std::string, std::vector<size_t>>*>(_structure);
                return hashtable->at(key);
            }
            case BTREE:
                break;

            default:
                throw std::runtime_error("Invalid index organization structure.");
        }
    }
};


// Esse metodo cria um indice para a tabela representada por mem em relacao ao campo de nome campo com a organizacao org.
template <typename R>
Index createIndex( std::string disk, MemoryWrapper<DataBlock<R>> mem, std::string campo, Organizacao org ) {
    Schema<R> schema;
    vhdf::readBlock(mem.getDiskId(), 0, &schema);

    Campo qCampo;
    bool found = false;
    for (int i = 0; i < schema.nCampos; i ++){
        if ( schema.campos[i].nm_campo == campo ) {
            qCampo = schema.campos[i];
            found = true;
        }
    }
    if (found = false) throw std::runtime_error("Campo não encontrado.");

    return createIndex( disk, mem, qCampo, org );        
}

// Esse metodo cria um indice para a tabela representada por mem em relacao ao campo c com a organizacao org.
template <typename R>
Index createIndex( std::string disk, MemoryWrapper<DataBlock<R>> mem, Campo c, Organizacao org ) {

    Schema<R> schema;
    vhdf::readBlock(mem.getDiskId(), 0, &schema);

    std::vector<std::pair<std::string, size_t>> entries = std::vector<std::pair<std::string, size_t>>();
    MemoryWrapper<DataBlock<R>> vhd(vhdf::openDisk(disk.c_str()));

    // Inserir registros de forma ordenada
    for (size_t i = schema.primeiro_bloco; i < schema.ultimo_bloco; i++) {
        vhd.loadBlock(i);
        for (size_t j = 0; j < schema.regs_por_bloco; j++) {
            entries.push_back( { getValorCampo(c, &vhd->getRegistro(j)), i } );
        }
    }

    Index index(org, entries);

    return index;
}

