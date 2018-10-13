#include <Campo.hpp>
#include <Util.hpp>
#include <HEAD.hpp>

enum TargetType { VALUE, SET, RANGE };

struct Target {
    Campo campo;
    TargetType tipo;
    std::vector<std::string> valor;
};

template <typename T>
std::vector<Target> parseQuery(HEAD<T> schema, std::vector<std::string> params) {

    std::vector<Target> targets = std::vector<Target>();

    //std::cout << "Parsing select fields..." << std::endl;

    for (int i = 0; i < params.size(); i++) {
        std::string str = params[i];
        unsigned int pos = str.find('=');
        std::string campo = str.substr(0, pos);
        std::string valor = str.substr(pos+1, str.length() - pos);
        bool found = false;

        for (int j = 0; j < schema.nCampos; j++) {
            if (strcmp(schema.campos[j].nm_campo, campo.c_str()) == 0) {
                Target target;
                target.campo = schema.campos[j];

                if (valor[0] == '[' && valor[valor.length()-1] == ']') {
                    target.valor = splitString(valor.substr(1, valor.length()-2), ':');
                    target.tipo = RANGE;
                }
                else if (valor[0] == '{' && valor[valor.length()-1] == '}') {
                    target.valor = splitString(valor.substr(1, valor.length()-2), ',');
                    target.tipo = SET;
                }
                else {
                    target.valor = {valor};
                    target.tipo = VALUE;
                }

                targets.push_back(target);
                found = true;
            }
        }
        if (!found) {
            throw std::invalid_argument("O campo "+campo+" nao consta da tabela");
        }
    }

    return targets;
};