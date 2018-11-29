//
// Created by Tuniks on 11/9/2018.
//
#pragma once
#include <Data.hpp>
#include <Util.hpp>
#include <vhdf.hpp>
#include <Campo.hpp>

class RegistroPartido
{
public:
    int NR_PARTIDO;
    char SG_PARTIDO[15];
    char NM_PARTIDO[50];

    RegistroPartido() {};
    explicit RegistroPartido(std::string csv);
    ~RegistroPartido() {};

    static constexpr int nPorBloco(){
        return (int)vhdf::BLOCK_SIZE/sizeof(RegistroPartido);
    };

    static constexpr int nCampos() {
        return 3;
    }

    static std::vector<Campo> initHEAD() {
        RegistroPartido reg = RegistroPartido();

        std::vector<Campo> campos(RegistroPartido::nCampos());
        // Iniciar campos
        campos[0] = Campo(sizeof(int), (size_t)&reg.NR_PARTIDO - (size_t)&reg, "NR_PARTIDO", INT);
        campos[1] = Campo(sizeof(char)*15, (size_t)&reg.SG_PARTIDO - (size_t)&reg, "SG_PARTIDO", CHAR);
        campos[2] = Campo(sizeof(char)*50, (size_t)&reg.NM_PARTIDO - (size_t)&reg, "NM_PARTIDO", CHAR);

        return campos;

    }
};
