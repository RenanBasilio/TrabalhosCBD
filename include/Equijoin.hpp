//
// Created by Tuniks on 11/10/2018.
//
#pragma once
#include <utility>
#include <iostream>
#include <cmath>
#include <unordered_map>

#include <Campo.hpp>
#include <DataBlock.hpp>
#include <MemoryWrapper.hpp>
#include <Registro.hpp>
#include <Schema.hpp>
#include <Util.hpp>
#include <Query.hpp>
#include <RegistroPartido.hpp>
#include <Ordered.hpp>
#include <Hash.hpp>
#include <Heap.hpp>

namespace Join {
    void runTests();
    void runTestsNested();

    std::vector<std::pair<Registro, RegistroPartido>> NESTEDJOIN(MemoryWrapper<DataBlock<Registro>> &mem1, MemoryWrapper<DataBlock<RegistroPartido>> &mem2, std::vector<std::string> params);
    std::vector<std::pair<Registro, RegistroPartido>> SORTMERGEJOIN(std::string join_attribute, int num_registros);
    std::vector<std::pair<Registro, RegistroPartido>> HASHJOIN(MemoryWrapper<DataBlock<Registro>> &mem1, MemoryWrapper<DataBlock<RegistroPartido>> &mem2, std::string join_attribute, int num_registros);

    // Printa conteúdos de uma tabela genérica
    template <typename T> 
    void printTable(MemoryWrapper<DataBlock<T>> &mem) {
    }

    template <> 
    void printTable<Registro>(MemoryWrapper<DataBlock<Registro>> &mem);

    template <> 
    void printTable<RegistroPartido>(MemoryWrapper<DataBlock<RegistroPartido>> &mem);
}