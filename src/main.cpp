#include <iostream>
#include <exception>
#include <vhdf.hpp>
#include <Campo.hpp>
#include <Parser/Registro.hpp>
#include <Parser/RegistroPartido.hpp>
#include <Parser/Parser.hpp>
#include <Schema.hpp>
#include <MemoryWrapper.hpp>
#include <TableUtils.hpp>
#include <Ordered.hpp>

int main() {
    if (Registro::nPorBloco() < 0) throw std::runtime_error("Erro: Tamanho do registro e grande de mais (>1 bloco)");

    std::cout << "Tamanho do registro e de " << sizeof(Registro) << " bytes." << std::endl;
    std::cout << "Tamanho do bloco e de " << vhdf::BLOCK_SIZE << " bytes (" << Registro::nPorBloco() << " registros por bloco)." << std::endl;

    std::cout << "Construindo tabelas iniciais..." << std::endl;

    MemoryWrapper tb_candidatos = createTable("tb_candidatos.vhd", 40000*sizeof(Registro), Registro::initHEAD(), HEAP);
    MemoryWrapper tb_partidos = createTable("tb_partidos.vhd", 40000*sizeof(RegistroPartido), RegistroPartido::initHEAD(), HEAP);
    MemoryWrapper tb_candidatos_ordenada = createTable("tb_candidatos_ordenada.vhd", 40000*sizeof(Registro), Registro::initHEAD(), ORDERED, "NM_CANDIDATO");
    MemoryWrapper tb_partidos_ordenada = createTable("tb_partidos_ordenada.vhd", 40000*sizeof(RegistroPartido), RegistroPartido::initHEAD(), ORDERED, "NR_PARTIDO");
    MemoryWrapper tb_candidatos_hash = createTable("tb_candidatos_hash.vhd", 40000*sizeof(Registro), Registro::initHEAD(), HASH, "NM_CANDIDATO");
    MemoryWrapper tb_partidos_hash = createTable("tb_partidos_hash.vhd", 40000*sizeof(RegistroPartido), RegistroPartido::initHEAD(), HASH, "NR_PARTIDO");

    std::ifstream ifs;
    ifs.open("consulta_cand_2018_BRASIL.csv", std::ifstream::in);
    std::string str;
    std::getline(ifs, str);

    parseStream<Registro>(tb_candidatos, ifs, 40000);
    ifs.close();

    std::cout << "Criou tabela tb_candidatos com " << tb_candidatos.getSchema().toString() << std::endl;

    ifs.open("partidos.csv", std::ifstream::in);
    std::getline(ifs, str);

    parseStream<RegistroPartido>(tb_partidos, ifs, 4000);
    ifs.close();

    std::cout << "Criou tabela tb_partidos com " << tb_partidos.getSchema().toString() << std::endl;

    std::vector<void*> inserts(1000);
    for ( size_t i = 0; ( i < ( inserts.size() / tb_candidatos.getSchema().regs_por_bloco ) ) && ( i + tb_candidatos.getSchema().primeiro_bloco <= tb_candidatos.getSchema().ultimo_bloco); i++ ) {
        tb_candidatos.loadBlock( i + tb_candidatos.getSchema().primeiro_bloco );
        for ( size_t j = 0; j < 10 ; j++ ) {
            inserts.at( i*10 + j ) = malloc( tb_candidatos.getSchema().tamanho );
            tb_candidatos->getRegistro( j, inserts.at( i*10 + j ) );
        }
    }
    Ordered::INSERT( tb_candidatos_ordenada, inserts );

    std::cout << "Criou tabela tb_candidatos_ordenada com " << tb_candidatos_ordenada.getSchema().toString() << std::endl;

    return 0;
}