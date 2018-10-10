#pragma once
#include <vhdf.hpp>
#include <bitset>

#include <Registro.hpp>

// Recupere um registro de um bloco
Registro getRegistro(int index, void* block);

// Escreve um registro em um bloco
void setRegistro(int index, void* block, const Registro& reg);