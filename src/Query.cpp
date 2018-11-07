#include <Query.hpp>

bool matchQuery ( const Target &query, const void* reg ) {
    switch (query.tipo) {
        case VALUE:
            if (comparaCampo(query.campo, reg, query.valor[0])) return true;
            break;
        case SET:
            for (int v = 0; v < query.valor.size(); v++)
                if (comparaCampo(query.campo, reg, query.valor[v])) 
                    return true;
            break;
        case RANGE:
            if (comparaCampo(query.campo, reg, query.valor[0], ">=") && 
                comparaCampo(query.campo, reg, query.valor[1], "<=")) return true;
            break;
        default:
            return false;
            break;
    }
    return false;
}
