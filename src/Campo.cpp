#include<stdexcept>
#include <Campo.hpp>
#include <Util.hpp>
#include <Registro.hpp>

bool comparaCampo( Campo campo, void* reg, std::string valor, const std::string &opr) {
    void* cmp_ptr = nullptr;

    enum OP { IGUAL, DIFF, MENOR, MAIOR, MEIG, MAIG };

    OP operacao;
    if (opr == "==") operacao = IGUAL;
    else if (opr == "!=") operacao = DIFF;
    else if (opr == "<") operacao = MENOR;
    else if (opr == ">") operacao = MAIOR;
    else if (opr == "<=") operacao = MEIG;
    else if (opr == ">=") operacao = MAIG;
    else throw std::invalid_argument("Operation not supported: '"+opr+"'");

    switch (campo.tipo) {
        case INT:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if (*reinterpret_cast<int*>(cmp_ptr) == std::stoi(valor)) return true;
                    break;
                case DIFF:
                    if (*reinterpret_cast<int*>(cmp_ptr) == std::stoi(valor)) return false;
                    break;
                case MENOR:
                    if (*reinterpret_cast<int*>(cmp_ptr) < std::stoi(valor)) return true;
                    break;
                case MAIOR:
                    if (*reinterpret_cast<int*>(cmp_ptr) > std::stoi(valor)) return true;
                    break;
                case MEIG:
                    if (*reinterpret_cast<int*>(cmp_ptr) <= std::stoi(valor)) return true;
                    break;
                case MAIG:
                    if (*reinterpret_cast<int*>(cmp_ptr) >= std::stoi(valor)) return true;
                    break;
            }
            break;
        case BIGINT:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if (*reinterpret_cast<size_t*>(cmp_ptr) == std::atoll(valor.c_str())) return true;
                    break;
                case DIFF:
                    if (*reinterpret_cast<size_t*>(cmp_ptr) == std::atoll(valor.c_str())) return false;
                    break;
                case MENOR:
                    if (*reinterpret_cast<size_t*>(cmp_ptr) < std::atoll(valor.c_str())) return true;
                    break;
                case MAIOR:
                    if (*reinterpret_cast<size_t*>(cmp_ptr) > std::atoll(valor.c_str())) return true;
                    break;
                case MEIG:
                    if (*reinterpret_cast<size_t*>(cmp_ptr) <= std::atoll(valor.c_str())) return true;
                    break;
                case MAIG:
                    if (*reinterpret_cast<size_t*>(cmp_ptr) >= std::atoll(valor.c_str())) return true;
                    break;
            }
            break;
        case CHAR:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if ( strncmp(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) == 0) return true;
                    break;
                case DIFF:
                    if ( strncmp(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) == 0) return false;
                    break;
                case MENOR:
                    if ( std::char_traits<char>::compare(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) < 0) return true;
                    break;
                case MAIOR:
                    if ( std::char_traits<char>::compare(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) > 0) return true;
                    break;
                case MEIG:
                    if ( std::char_traits<char>::compare(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) <= 0) return true;
                    break;
                case MAIG:
                    if ( std::char_traits<char>::compare(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) >= 0) return true;
                    break;
            }
            break;
        /*
        case DATA: {
            Data* data_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
            if (*data_ptr == Data(targets[k].second)) match++;
            break;
        }
        case HORA: {
            Data* hora_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
            if (*hora_ptr == Hora(targets[k].second)) match++;
            break;
        }
        case TIMESTAMP: {
            Timestamp* tstamp_ptr = reinterpret_cast<char*>(&reg) + targets[k].first.pos_relativa;
            if (*tstamp_ptr == Timestamp(targets[k].second)) match++;
            break;
        }
        */
        case BOOL:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            switch (operacao) {
                case IGUAL:
                    if (*reinterpret_cast<bool*>(cmp_ptr) == SNtoBool(valor)) return true;
                    break;
                case DIFF:
                    if (*reinterpret_cast<bool*>(cmp_ptr) == SNtoBool(valor)) return false;
                    break;
                default:
                    throw std::invalid_argument("Operacao invalida para campo de tipo BOOL ('"+opr+")");
            }
            break;
        default:
            return false;
    }
    return false;
}