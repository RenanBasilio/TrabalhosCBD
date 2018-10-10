# TrabalhosCBD
Trabalhos para a disciplina Construção de Banco de Dados, ministrada pelo professor Milton Ramirez no período de 2018-2.

A base de dados inclusa neste repositório foi obtida no endereço 
http://agencia.tse.jus.br/estatistica/sead/odsele/consulta_cand/consulta_cand_2018.zip em 09/10/2018.

## Compilação
Pré-Requisitos:

* GNU Make ou Autotools ou CMake
* Compilador de C++ com suporte para C++11

### Make
Se utilizando uma versão previamente empacotada deste programa, execute os seguintes comandos em um terminal:

```
mkdir build && cd build
../configure
make
make install
```

### CMake
Um CMakeLists.txt se encontra juntamente ao código fonte. Ele pode ser utilizado para gerar os scripts de compilação conjuntamente com o programa CMake.

```
cmake . -Bbuild
cmake --build build --config Release
```

A flag ```--config Release``` é opcional em sistemas operacionais baseados em Unix.

### Autotools
Se estiver clonando este repositório diretamente, os arquivos de distribuição preconfigurados não estarão inclusos. Para gera-los utilizando o Autotools, execute os seguintes comandos em um terminal:

```
aclocal
autoconf
automake --add-missing
```
Após executar esses comandos, siga as instruções para [compilar usando Make](#make).
