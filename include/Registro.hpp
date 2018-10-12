#pragma once
#include <Data.hpp>
#include <Util.hpp>
#include <vhdf.hpp>
#include <Campo.hpp>

class Registro
{
public:
    int ANO_ELEICAO;
    int CD_TIPO_ELEICAO;
    //char NM_TIPO_ELEICAO[20];
    int NR_TURNO;
    int CD_ELEICAO;
    //char DS_ELEICAO[30];
    Data DT_ELEICAO;
    char TP_ABRANGENCIA[8];
    char SG_UF[2];
    char SG_UE[2];
    //char NM_UE[20];
    int CD_CARGO;
    //char DS_CARGO[20];
    size_t SQ_CANDIDATO;                       // Surrogate
    int NR_CANDIDATO;
    char NM_CANDIDATO[70];
    char NM_URNA_CANDIDATO[30];
    char NM_SOCIAL_CANDIDATO[40];
    char NR_CPF_CANDIDATO[11];                                 // Outro candidato para chave primaria
    char NM_EMAIL[60];
    int CD_SITUACAO_CANDIDATURA;
    //char DS_SITUACAO_CANDIDATURA[10];
    int CD_DETALHE_SITUACAO_CAND;
    //char DS_DETALHE_SITUACAO_CAND[30];
    char TP_AGREMIACAO[15];
    int NR_PARTIDO;
    //char SG_PARTIDO[15];
    //char NM_PARTIDO[50];
    size_t SQ_COLIGACAO;
    //char NM_COLIGACAO[55];
    //char DS_COMPOSICAO_COLIGACAO[150];
    int CD_NACIONALIDADE;
    //char DS_NACIONALIDADE[20];  // Double check
    char SG_UF_NASCIMENTO[2];
    int CD_MUNICIPIO_NASCIMENTO;
    //char NM_MUNICIPIO_NASCIMENTO[35];
    Data DT_NASCIMENTO;
    int NR_IDADE_DATA_POSSE;
    char NR_TITULO_ELEITORAL_CANDIDATO[12];
    int CD_GENERO;
    //char DS_GENERO[9];
    int CD_GRAU_INSTRUCAO;
    //char DS_GRAU_INSTRUCAO[29];
    int CD_ESTADO_CIVIL;
    //char DS_ESTADO_CIVIL[25];
    int CD_COR_RACA;
    //char DS_COR_RACA[20];       // Double check
    int CD_OCUPACAO;
    //char DS_OCUPACAO[70];
    int VR_MAX_DESPESA_CAMPANHA;
    int CD_SIT_TOT_TURNO;
    //char DS_SIT_TOT_TURNO[6];
    bool ST_REELEICAO;
    bool ST_DECLARAR_BENS;
    int NR_PROTOCOLO_CANDIDATURA;
    size_t NR_PROCESSO;
    
    Registro() {};
    explicit Registro(std::string csv);
    ~Registro() {};

    static constexpr int nPorBloco(){
        return (int)vhdf::BLOCK_SIZE/sizeof(Registro);
    };

    static constexpr int nCampos() {
        return 38;
    }

    static std::vector<Campo> initHEAD() {

        std::vector<Campo> campos(Registro::nCampos());
        // Iniciar campos
        campos[0] = Campo(sizeof(int), 0, "ANO_ELEICAO", INT);
        campos[1] = Campo(sizeof(int), 0, "CD_TIPO_ELEICAO", INT);
        campos[2] = Campo(sizeof(int), 0, "NR_TURNO", INT);
        campos[3] = Campo(sizeof(int), 0, "CD_ELEICAO", INT);
        campos[4] = Campo(sizeof(Data), 0, "DT_ELEICAO", DATA);
        campos[5] = Campo(sizeof(char)*8, 0, "TP_ABRANGENCIA", CHAR);
        campos[6] = Campo(sizeof(char)*2, 0, "SG_UF", CHAR);
        campos[7] = Campo(sizeof(char)*2, 0, "SG_UE", CHAR);
        campos[8] = Campo(sizeof(int), 0, "CD_CARGO", INT);
        campos[9] = Campo(sizeof(size_t), 0, "SQ_CANDIDATO", BIGINT);
        campos[10] = Campo(sizeof(int), 0, "NR_CANDIDATO", INT);
        campos[11] = Campo(sizeof(char)*70, 0, "NM_CANDIDATO", CHAR);
        campos[12] = Campo(sizeof(char)*30, 0, "NM_URNA_CANDIDATO", CHAR);
        campos[13] = Campo(sizeof(char)*40, 0, "NM_SOCIAL_CANDIDATO", CHAR);
        campos[14] = Campo(sizeof(char)*11, 0, "NR_CPF_CANDIDATO", CHAR);
        campos[15] = Campo(sizeof(char)*60, 0, "NM_EMAIL", CHAR);
        campos[16] = Campo(sizeof(int), 0, "CD_SITUACAO_CANDIDATURA", INT);
        campos[17] = Campo(sizeof(int), 0, "CD_DETALHE_SITUACAO_CAND", INT);
        campos[18] = Campo(sizeof(char)*15, 0, "TP_AGREMIACAO", CHAR);
        campos[19] = Campo(sizeof(int), 0, "NR_PARTIDO", INT);
        campos[20] = Campo(sizeof(size_t), 0, "SQ_COLIGACAO", BIGINT);
        campos[21] = Campo(sizeof(int), 0, "CD_NACIONALIDADE", INT);
        campos[22] = Campo(sizeof(char)*2, 0, "SG_UF_NASCIMENTO", CHAR);
        campos[23] = Campo(sizeof(int), 0, "CD_MUNICIPIO_NASCIMENTO", INT);
        campos[24] = Campo(sizeof(Data), 0, "DT_NASCIMENTO", DATA);
        campos[25] = Campo(sizeof(int), 0, "NR_IDADE_DATA_POSSE", INT);
        campos[26] = Campo(sizeof(char)*12, 0, "NR_TITULO_ELEITORAL_CANDIDATO", CHAR);
        campos[27] = Campo(sizeof(int), 0, "CD_GENERO", INT);
        campos[28] = Campo(sizeof(int), 0, "CD_GRAU_INSTRUCAO", INT);
        campos[29] = Campo(sizeof(int), 0, "CD_ESTADO_CIVIL", INT);
        campos[30] = Campo(sizeof(int), 0, "CD_COR_RACA", INT);
        campos[31] = Campo(sizeof(int), 0, "CD_OCUPACAO", INT);
        campos[32] = Campo(sizeof(int), 0, "VR_MAX_DESPESA_CAMPANHA", INT);
        campos[33] = Campo(sizeof(int), 0, "CD_SIT_TOT_TURNO", INT);
        campos[34] = Campo(sizeof(bool), 0, "ST_REELEICAO", BOOL);
        campos[35] = Campo(sizeof(bool), 0, "ST_DECLARAR_BENS", BOOL);
        campos[36] = Campo(sizeof(int), 0, "NR_PROTOCOLO_CANDIDATURA", INT);
        campos[37] = Campo(sizeof(size_t), 0, "NR_PROCESSO", BIGINT);

        // Atualizar posições relativas
        int rel = 0;
        for(int i=1; i < campos.size(); i++) {
            rel += campos[i-1].tamanho;
            campos[i].pos_relativa = rel;
        }

        return campos;

    }
};
