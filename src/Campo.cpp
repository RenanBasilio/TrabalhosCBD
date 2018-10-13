#include <Campo.hpp>
#include <Util.hpp>
#include <Registro.hpp>

bool comparaCampo( Campo campo, void* reg, std::string valor ) {
    void* cmp_ptr = nullptr;
    switch (campo.tipo) {
        case INT:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            if (*reinterpret_cast<int*>(cmp_ptr) == std::stoi(valor)) return true;
            break;
        case BIGINT:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            if (*reinterpret_cast<size_t*>(cmp_ptr) == std::atoll(valor.c_str())) return true;
            break;
        case CHAR:
            cmp_ptr = reinterpret_cast<char*>(reg) + campo.pos_relativa;
            if ( strncmp(reinterpret_cast<char*>(cmp_ptr), valor.c_str(), campo.tamanho) == 0) return true;
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
            if (*reinterpret_cast<bool*>(cmp_ptr) == SNtoBool(valor)) return true;
            break;
        default:
            return false;
    }
    return false;
}