#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include "enums.h"

typedef struct {
    int inode;
    char nome[MAX_NOME];
    int tamanho;
    TipoDado tipo_dado;
    time_t criado_em;
    time_t modificado_em;
    time_t acessado_em;
    int permissoes;
} Metadados;

typedef struct NoArvore {
    Metadados meta;
    TipoNo tipo_no;
    char conteudo[MAX_CONTEUDO];

    struct NoArvore* pai;
    struct NoArvore* esquerda;
    struct NoArvore* direita;
    struct NoArvore* filhos;
} NoArvore;

// Variáveis globais
extern int contador_inodes;
extern NoArvore* raiz;
extern NoArvore* diretorio_atual;

#endif