//
// Created by Tuniks on 11/9/2018.
//

#include <RegistroPartido.hpp>
#include <cstring>

RegistroPartido::RegistroPartido(std::string csv) {
    std::vector<std::string> splits = splitString(csv, ';');
    for (int i = 0; i < splits.size(); i++) {
        splits[i] = splits[i].substr(1, splits[i].length()-2);
    }

    NR_PARTIDO = stoi(splits[27]);
    strncpy(SG_PARTIDO, splits[28].c_str(), sizeof(SG_PARTIDO));
    strncpy(NM_PARTIDO, splits[29].c_str(), sizeof(NM_PARTIDO));
}