#ifndef BLOCOS_H
#define BLOCOS_H

#include "enums.h"

#define TAMANHO_DISCO 1000 
#define TAMANHO_BLOCO 32
#define BLOCO_LIVRE 0
#define BLOCO_OCUPADO 1

typedef struct {
    int inode;
    int posicao_no_arquivo;
} BlocoInfo;


typedef struct {
    char dados[TAMANHO_DISCO][TAMANHO_BLOCO];
    BlocoInfo info[TAMANHO_DISCO];
    int status[TAMANHO_DISCO];
    int blocos_livres;
} DiscoSimulado;

void inicializarDisco();
int alocarBlocos(int inode, int blocos_necessarios);
void liberarBlocos(int inode);
int calcularBlocosNecessarios(int tamanho_arquivo);
void escreverArquivoNoBlocos(int inode, const char* conteudo);
void exibirEstatisticasDisco();
int encontrarBlocosContiguos(int quantidade);

#endif