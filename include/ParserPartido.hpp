//
// Created by Tuniks on 11/9/2018.
//

#pragma once
#include <fstream>
#include <string>
#include <vhdf.hpp>
#include <RegistroPartido.hpp>
#include <DataBlock.hpp>

void parsePartyStream(std::ifstream& ifs, int vhd, int blockoffset);


