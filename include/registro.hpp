#pragma once
#include <memory>
#include <Data.hpp>

class Registro
{
private:
    int ANO_ELEICAO;
    int CD_TIPO_ELEICAO;
    char NM_TIPO_ELEICAO[20];
    int NR_TURNO;
    int CD_ELEICAO;
    char DS_ELEICAO[30];
    Data DT_ELEICAO;
    char TP_ABRANGENCIA[8];
    char SG_UF[2];
    char SG_UE[2];
    char NM_UE[20];
    int CD_CARGO;
    char DS_CARGO[20];
    int SQ_CANDIDATO;                       // Surrogate
    int NR_CANDIDATO;
    char NM_CANDIDATO[70];
    char NM_URNA_CANDIDATO[30];
    char NM_SOCIAL_CANDIDATO[40];
    int NR_CPF_CANDIDATO;                   // Outro candidato para chave primaria
    char NM_EMAIL[60];
    int CD_SITUACAO_CANDIDATURA;
    char DS_SITUACAO_CANDIDATURA[10];
    int CD_DETALHE_SITUACAO_CAND;
    char DS_DETALHE_SITUAACAO_CAND[30];
    char TP_AGREMIACAO[15];
    int NR_PARTIDO;
    char SG_PARTIDO[15];
    char NM_PARTIDO[50];
    int SQ_COLIGACAO;
    char NM_COLIGACAO[55];
    char DS_COMPOSICAO_COLIGACAO[150];
    int CD_NACIONALIDADE;
    char DS_NACIONALIDADE;
    char SG_UF_NASCIMENTO[2];
    int CD_MUNICIPIO_NASCIMENTO;
    char NM_MUNICIPIO_NASCIMENTO[35];
    Data DT_NASCIMENTO;
    int NR_IDADE_DATA_POSSE;
    int NR_TITULO_ELEITORAL_CANDIDATO;
    int CD_GENERO;
    char DS_GENERO[9];
    int CD_GRAU_INSTRUCAO;
    char DS_GRAU_INSTRUCAO[29];
    int CD_ESTADO_CIVIL;
    char DS_ESTADO_CIVIL[25];
    int CD_COR_RACA;
    char DS_COR_RACA;
    int CD_OCUPACAO;
    char DS_OCUPACAO[70];
    int VR_MAX_DESPESA_CAMPANHA;
    int CD_SIT_TOT_TURNO;
    char DS_SIT_TOT_TURNO[6];
    bool ST_REELEICAO;
    bool ST_DECLARAR_BENS;
    int NR_PROTOCOLO_CANDIDATURA;
    long long unsigned int NR_PROCESSO;
    
public:
    Registro();
    ~Registro();
};

Registro::Registro()
{
}

Registro::~Registro()
{
}
