#pragma once
#include <Data.hpp>

class HEAD
{
private:
    enum organizacao { HEAP, ORDERED, HASH };
    // Vetor de tamanhos e localizações relativas dos campos e do registro (usar função initRegistro abaixo)
    // Ponteiros para estruturas secundárias como o identificador de registros deletados
    // Timestamps relevantes à base
public:
    Campo[38] campos;
    size_t registros_deletados;
    Timestamp criacao, alteracao;
}


void initRegistro(int pos_rel_registro) {
    // Iniciar campos
    campos[0] = Campo(sizeof(int), 0, "ANO_ELEICAO");
    campos[1] = Campo(sizeof(int), 0, "CD_TIPO_ELEICAO");
    campos[2] = Campo(sizeof(int), 0, "NR_TURNO");
    campos[3] = Campo(sizeof(int), 0, "CD_ELEICAO");
    campos[4] = Campo(sizeof(Data), 0, "DT_ELEICAO");
    campos[5] = Campo(sizeof(char)*8, 0, "TP_ABRANGENCIA");
    campos[6] = Campo(sizeof(char)*2, 0, "SG_UF");
    campos[7] = Campo(sizeof(char)*2, 0, "SG_UE");
    campos[8] = Campo(sizeof(int), 0, "CD_CARGO");
    campos[9] = Campo(sizeof(size_t), 0, "SQ_CANDIDATO");
    campos[10] = Campo(sizeof(int), 0, "NR_CANDIDATO");
    campos[11] = Campo(sizeof(char)*70, 0, "NM_CANDIDATO");
    campos[12] = Campo(sizeof(char)*30, 0, "NM_URNA_CANDIDATO");
    campos[13] = Campo(sizeof(char)*40, 0, "NM_SOCIAL_CANDIDATO");
    campos[14] = Campo(sizeof(char)*11, 0, "NR_CPF_CANDIDATO");
    campos[15] = Campo(sizeof(char)*60, 0, "NM_EMAIL");
    campos[16] = Campo(sizeof(int), 0, "CD_SITUACAO_CANDIDATURA");
    campos[17] = Campo(sizeof(int), 0, "CD_DETALHE_SITUACAO_CAND;");
    campos[18] = Campo(sizeof(char)*15, 0, "TP_AGREMIACAO");
    campos[19] = Campo(sizeof(int), 0, "NR_PARTIDO");
    campos[20] = Campo(sizeof(size_t), 0, "SQ_COLIGACAO");
    campos[21] = Campo(sizeof(int), 0, "CD_NACIONALIDADE");
    campos[22] = Campo(sizeof(char)*2, 0, "SG_UF_NASCIMENTO");
    campos[23] = Campo(sizeof(int), 0, "CD_MUNICIPIO_NASCIMENTO");
    campos[24] = Campo(sizeof(Data), 0, "DT_NASCIMENTO");
    campos[25] = Campo(sizeof(int), 0, "NR_IDADE_DATA_POSSE");
    campos[26] = Campo(sizeof(char)*12, 0, "NR_TITULO_ELEITORAL_CANDIDATO");
    campos[27] = Campo(sizeof(int), 0, "CD_GENERO");
    campos[28] = Campo(sizeof(int), 0, "CD_GRAU_INSTRUCAO");
    campos[29] = Campo(sizeof(int), 0, "CD_ESTADO_CIVIL");
    campos[30] = Campo(sizeof(int), 0, "CD_COR_RACA");
    campos[31] = Campo(sizeof(int), 0, "CD_OCUPACAO");
    campos[32] = Campo(sizeof(int), 0, "VR_MAX_DESPESA_CAMPANHA");
    campos[33] = Campo(sizeof(int), 0, "CD_SIT_TOT_TURNO");
    campos[34] = Campo(sizeof(bool), 0, "ST_REELEICAO");
    campos[35] = Campo(sizeof(bool), 0, "ST_DECLARAR_BENS");
    campos[36] = Campo(sizeof(int), 0, "NR_PROTOCOLO_CANDIDATURA");
    campos[37] = Campo(sizeof(size_t), 0, "NR_PROCESSO");

    // Atualizar posições relativas
    int rel = pos_rel_registro;
    for(int i=1; i < sizeof(campos); i++) {
       rel += campos[i-1].tamanho;
       campos[i].pos_relativa = rel;
    }
 }
