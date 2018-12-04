//
// Created by Tuniks on 11/10/2018.
//

#pragma once
#include <utility>
#include <iostream>
#include <cmath>

#include <Campo.hpp>
#include <DataBlock.hpp>
#include <MemoryWrapper.hpp>
#include <Registro.hpp>
#include <Schema.hpp>
#include <Util.hpp>
#include <Query.hpp>
#include <RegistroPartido.hpp>
#include <Ordered.hpp>

namespace Join {
    void runTests();

    std::vector<std::pair<Registro, RegistroPartido>> NESTEDJOIN(MemoryWrapper<DataBlock<Registro>> &mem1, MemoryWrapper<DataBlock<RegistroPartido>> &mem2, std::vector<std::string> params);
    std::vector<std::pair<Registro, RegistroPartido>> SORTMERGEJOIN(std::string join_attribute, int num_registros);

}