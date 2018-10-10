#include <Registro.hpp>

Registro::Registro(std::string csv) {
    std::vector<std::string> splits = splitString(csv, ';');
    for (int i = 0; i < splits.size(); i++) {
        splits[i] = splits[i].substr(1, splits[i].length()-2);
    }
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
    SQ_CANDIDATO = atoll(splits[15].c_str());
    NR_CANDIDATO = stoi(splits[16]);
    strcpy(NM_CANDIDATO, splits[17].c_str());
    strcpy(NM_URNA_CANDIDATO, splits[18].c_str());
    strcpy(NM_SOCIAL_CANDIDATO, splits[19].c_str());
    strcpy(NR_CPF_CANDIDATO, splits[20].c_str());
    strcpy(NM_EMAIL, splits[21].c_str());
    CD_SITUACAO_CANDIDATURA = stoi(splits[22]);
    strcpy(DS_SITUACAO_CANDIDATURA, splits[23].c_str());
    CD_DETALHE_SITUACAO_CAND = stoi(splits[24]);
    strcpy(DS_DETALHE_SITUAACAO_CAND, splits[25].c_str());
    strcpy(TP_AGREMIACAO, splits[26].c_str());
    NR_PARTIDO = stoi(splits[27]);
    strcpy(SG_PARTIDO, splits[28].c_str());
    strcpy(NM_PARTIDO, splits[29].c_str());
    SQ_COLIGACAO = atoll(splits[30].c_str());
    strcpy(NM_COLIGACAO, splits[31].c_str());
    strcpy(DS_COMPOSICAO_COLIGACAO, splits[32].c_str());
    CD_NACIONALIDADE = stoi(splits[33]);
    strcpy(DS_NACIONALIDADE, splits[34].c_str());
    strcpy(SG_UF_NASCIMENTO, splits[35].c_str());
    CD_MUNICIPIO_NASCIMENTO = stoi(splits[36]);
    strcpy(NM_MUNICIPIO_NASCIMENTO, splits[37].c_str());
    DT_NASCIMENTO = Data(splits[38]);
    NR_IDADE_DATA_POSSE = stoi(splits[39]);
    strcpy(NR_TITULO_ELEITORAL_CANDIDATO, splits[40].c_str());
    CD_GENERO = stoi(splits[41]);
    strcpy(DS_GENERO, splits[42].c_str());
    CD_GRAU_INSTRUCAO = stoi(splits[43]);
    strcpy(DS_GRAU_INSTRUCAO, splits[44].c_str());
    CD_ESTADO_CIVIL = stoi(splits[45]);
    strcpy(DS_ESTADO_CIVIL, splits[46].c_str());
    CD_COR_RACA = stoi(splits[47]);
    strcpy(DS_COR_RACA, splits[48].c_str());
    CD_OCUPACAO = stoi(splits[49]);
    strcpy(DS_OCUPACAO, splits[50].c_str());
    VR_MAX_DESPESA_CAMPANHA = stoi(splits[51]);
    CD_SIT_TOT_TURNO = stoi(splits[52]);
    strcpy(DS_SIT_TOT_TURNO, splits[53].c_str());
    ST_REELEICAO = SNtoBool(splits[54]);
    ST_DECLARAR_BENS = SNtoBool(splits[55]);
    NR_PROTOCOLO_CANDIDATURA = stoi(splits[56]);
    NR_PROCESSO = atoll(splits[57].c_str());
}

