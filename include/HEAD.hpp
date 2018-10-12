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
}


char* initRegistro(int pos_rel_registro) {
    char[38] arr;

    // Iniciar campos
    arr[0] = Campo(sizeof(int), 0, "ANO_ELEICAO");
    arr[1] = Campo(sizeof(int), 0, 'CD_TIPO_ELEICAO');
    arr[2] = Campo(sizeof(int), 0, 'NR_TURNO');
    arr[3] = Campo(sizeof(int), 0, 'CD_ELEICAO');
    arr[4] = Campo(sizeof(Data), 0, 'DT_ELEICAO');       
    arr[5] = Campo(sizeof(char)*8, 0, 'TP_ABRANGENCIA');
    arr[6] = Campo(sizeof(char)*2, 0, 'SG_UF');
    arr[7] = Campo(sizeof(char)*2, 0, 'SG_UE');
    arr[8] = Campo(sizeof(int), 0, 'CD_CARGO');
    arr[9] = Campo(sizeof(size_t), 0, 'SQ_CANDIDATO');
    arr[10] = Campo(sizeof(int), 0, 'NR_CANDIDATO');
    arr[11] = Campo(sizeof(char)*70, 0, 'NM_CANDIDATO');
    arr[12] = Campo(sizeof(char)*30, 0, 'NM_URNA_CANDIDATO');
    arr[13] = Campo(sizeof(char)*40, 0, 'NM_SOCIAL_CANDIDATO');
    arr[14] = Campo(sizeof(char)*11, 0, 'NR_CPF_CANDIDATO');
    arr[15] = Campo(sizeof(char)*60, 0, 'NM_EMAIL');
    arr[16] = Campo(sizeof(int), 0, 'CD_SITUACAO_CANDIDATURA');
    arr[17] = Campo(sizeof(int), 0, 'CD_DETALHE_SITUACAO_CAND;');
    arr[18] = Campo(sizeof(char)*15, 0, 'TP_AGREMIACAO');
    arr[19] = Campo(sizeof(int), 0, 'NR_PARTIDO');
    arr[20] = Campo(sizeof(size_t), 0, 'SQ_COLIGACAO');
    arr[21] = Campo(sizeof(int), 0, 'CD_NACIONALIDADE');
    arr[22] = Campo(sizeof(char)*2, 0, 'SG_UF_NASCIMENTO'); 
    arr[23] = Campo(sizeof(int), 0, 'CD_MUNICIPIO_NASCIMENTO');
    arr[24] = Campo(sizeof(Data), 0, 'DT_NASCIMENTO');       
    arr[25] = Campo(sizeof(int), 0, 'NR_IDADE_DATA_POSSE');
    arr[26] = Campo(sizeof(char)*12, 0, 'NR_TITULO_ELEITORAL_CANDIDATO'); 
    arr[27] = Campo(sizeof(int), 0, 'CD_GENERO');
    arr[28] = Campo(sizeof(int), 0, 'CD_GRAU_INSTRUCAO');
    arr[29] = Campo(sizeof(int), 0, 'CD_ESTADO_CIVIL');
    arr[30] = Campo(sizeof(int), 0, 'CD_COR_RACA');
    arr[31] = Campo(sizeof(int), 0, 'CD_OCUPACAO');
    arr[32] = Campo(sizeof(int), 0, 'VR_MAX_DESPESA_CAMPANHA');
    arr[33] = Campo(sizeof(int), 0, 'CD_SIT_TOT_TURNO');
    arr[34] = Campo(sizeof(bool), 0, 'ST_REELEICAO');
    arr[35] = Campo(sizeof(bool), 0, 'ST_DECLARAR_BENS');
    arr[36] = Campo(sizeof(int), 0, 'NR_PROTOCOLO_CANDIDATURA');
    arr[37] = Campo(sizeof(size_t), 0, 'NR_PROCESSO');

    // Atualizar posições relativas
    int rel = pos_rel_registro;
    for(i=1; i < sizeof(arr); i++) {
       rel += arr[i-1].tamanho;
       arr[i].pos_relativa = rel;
    } 

    return arr;
 }
