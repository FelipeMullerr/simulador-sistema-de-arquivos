#ifndef ENUMS_H
#define ENUMS_H

#include <time.h>

#define MAX_NOME 100
#define MAX_CONTEUDO 1024

typedef enum {ARQUIVO, DIRETORIO} TipoNo;

typedef enum {CARACTERES, PROGRAMA, CSV, DESCONHECIDO} TipoDado;

typedef enum {OWNER, GROUP, OTHER} Usuario;

#endif