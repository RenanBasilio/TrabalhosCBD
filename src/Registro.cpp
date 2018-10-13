#include <Registro.hpp>
#include <cstring>

Registro::Registro(std::string csv) {
    std::vector<std::string> splits = splitString(csv, ';');
    for (int i = 0; i < splits.size(); i++) {
        splits[i] = splits[i].substr(1, splits[i].length()-2);
    }
    ANO_ELEICAO = stoi(splits[2]);
    CD_TIPO_ELEICAO = stoi(splits[3]);
    //strncpy(NM_TIPO_ELEICAO, splits[4].c_str(), sizeof(NM_TIPO_ELEICAO));
    NR_TURNO = stoi(splits[5]);
    CD_ELEICAO = stoi(splits[6]);
    //strncpy(DS_ELEICAO, splits[7].c_str(), sizeof(DS_ELEICAO));
    DT_ELEICAO = Data(splits[8]);
    strncpy(TP_ABRANGENCIA, splits[9].c_str(), sizeof(TP_ABRANGENCIA));
    strncpy(SG_UF, splits[10].c_str(), sizeof(SG_UF));
    strncpy(SG_UE, splits[11].c_str(), sizeof(SG_UE));
    //strncpy(NM_UE, splits[12].c_str(), sizeof(NM_UE));
    CD_CARGO = stoi(splits[13]);
    //strncpy(DS_CARGO, splits[14].c_str(), sizeof(DS_CARGO));
    SQ_CANDIDATO = atoll(splits[15].c_str());
    NR_CANDIDATO = stoi(splits[16]);
    strncpy(NM_CANDIDATO, splits[17].c_str(), sizeof(NM_CANDIDATO));
    strncpy(NM_URNA_CANDIDATO, splits[18].c_str(), sizeof(NM_URNA_CANDIDATO));
    strncpy(NM_SOCIAL_CANDIDATO, splits[19].c_str(), sizeof(NM_SOCIAL_CANDIDATO));
    strncpy(NR_CPF_CANDIDATO, splits[20].c_str(), sizeof(NR_CPF_CANDIDATO));
    strncpy(NM_EMAIL, splits[21].c_str(), sizeof(NM_EMAIL));
    CD_SITUACAO_CANDIDATURA = stoi(splits[22]);
    //strncpy(DS_SITUACAO_CANDIDATURA, splits[23].c_str(), sizeof(DS_SITUACAO_CANDIDATURA));
    CD_DETALHE_SITUACAO_CAND = stoi(splits[24]);
    //strncpy(DS_DETALHE_SITUACAO_CAND, splits[25].c_str(), sizeof(DS_DETALHE_SITUACAO_CAND));
    strncpy(TP_AGREMIACAO, splits[26].c_str(), sizeof(TP_AGREMIACAO));
    NR_PARTIDO = stoi(splits[27]);
    //strncpy(SG_PARTIDO, splits[28].c_str(), sizeof(SG_PARTIDO));
    //strncpy(NM_PARTIDO, splits[29].c_str(), sizeof(NM_PARTIDO));
    SQ_COLIGACAO = atoll(splits[30].c_str());
    //strncpy(NM_COLIGACAO, splits[31].c_str(), sizeof(NM_COLIGACAO));
    //strncpy(DS_COMPOSICAO_COLIGACAO, splits[32].c_str(), sizeof(DS_COMPOSICAO_COLIGACAO));
    CD_NACIONALIDADE = stoi(splits[33]);
    //strncpy(DS_NACIONALIDADE, splits[34].c_str(), sizeof(DS_NACIONALIDADE));
    strncpy(SG_UF_NASCIMENTO, splits[35].c_str(), sizeof(SG_UF_NASCIMENTO));
    CD_MUNICIPIO_NASCIMENTO = stoi(splits[36]);
    //strncpy(NM_MUNICIPIO_NASCIMENTO, splits[37].c_str(), sizeof(NM_MUNICIPIO_NASCIMENTO));
    DT_NASCIMENTO = Data(splits[38]);
    NR_IDADE_DATA_POSSE = stoi(splits[39]);
    strncpy(NR_TITULO_ELEITORAL_CANDIDATO, splits[40].c_str(), sizeof(NR_TITULO_ELEITORAL_CANDIDATO));
    CD_GENERO = stoi(splits[41]);
    //strncpy(DS_GENERO, splits[42].c_str(), sizeof(DS_GENERO));
    CD_GRAU_INSTRUCAO = stoi(splits[43]);
    //strncpy(DS_GRAU_INSTRUCAO, splits[44].c_str(), sizeof(DS_GRAU_INSTRUCAO));
    CD_ESTADO_CIVIL = stoi(splits[45]);
    //strncpy(DS_ESTADO_CIVIL, splits[46].c_str(), sizeof(DS_ESTADO_CIVIL));
    CD_COR_RACA = stoi(splits[47]);
    //strncpy(DS_COR_RACA, splits[48].c_str(), sizeof(DS_COR_RACA));
    CD_OCUPACAO = stoi(splits[49]);
    //strncpy(DS_OCUPACAO, splits[50].c_str(), sizeof(DS_OCUPACAO));
    VR_MAX_DESPESA_CAMPANHA = stoi(splits[51]);
    CD_SIT_TOT_TURNO = stoi(splits[52]);
    //strncpy(DS_SIT_TOT_TURNO, splits[53].c_str(), sizeof(DS_SIT_TOT_TURNO));
    ST_REELEICAO = SNtoBool(splits[54]);
    ST_DECLARAR_BENS = SNtoBool(splits[55]);
    NR_PROTOCOLO_CANDIDATURA = stoi(splits[56]);
    strncpy(NR_PROCESSO, splits[57].c_str(), sizeof(NR_PROCESSO));
}

