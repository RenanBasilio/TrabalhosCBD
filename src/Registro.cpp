#include <Registro.hpp>

Registro::Registro(std::string csv) {
    std::vector<std::string> splits = splitString(csv, ';');
    ANO_ELEICAO = stoi(splits[2]);
    CD_TIPO_ELEICAO = stoi(splits[3]);
    strcpy(NM_TIPO_ELEICAO, splits[4].c_str());
    NR_TURNO = stoi(splits[5]);
    CD_ELEICAO = stoi(splits[6]);
    strcpy(DS_ELEICAO, splits[7].c_str());
    DT_ELEICAO = Data(splits[8]);
    strcpy(TP_ABRANGENCIA, splits[9].c_str());
    strcpy(SG_UF, splits[10].c_str());
    strcpy(SG_UE, splits[11].c_str());
    strcpy(NM_UE, splits[12].c_str());
    CD_CARGO = stoi(splits[13]);
    strcpy(DS_CARGO, splits[14].c_str());
    
}