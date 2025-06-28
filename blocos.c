#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blocos.h"

DiscoSimulado disco;

void inicializarDisco() {
    for (int i = 0; i < TAMANHO_DISCO; i++) {
        disco.status[i] = BLOCO_LIVRE;
        disco.info[i].inode = -1;
        disco.info[i].posicao_no_arquivo = -1;
        for (int j = 0; j < TAMANHO_BLOCO; j++) {
            disco.dados[i][j] = 0;
        }
    }
    disco.blocos_livres = TAMANHO_DISCO;
    printf("Disco simulado inicializado: %d blocos de %d bytes cada\n", TAMANHO_DISCO, TAMANHO_BLOCO);
}

int calcularBlocosNecessarios(int tamanho_arquivo) {
    if (tamanho_arquivo == 0) return 0;
    return (tamanho_arquivo + TAMANHO_BLOCO - 1) / TAMANHO_BLOCO;
}

int encontrarBlocosContiguos(int quantidade) {
    if (quantidade == 0) return -1;
    
    // Procura por blocos contíguos livres
    for (int i = 0; i <= TAMANHO_DISCO - quantidade; i++) {
        int contiguos = 0;
        // Verifica se a partir da posição i, existem blocos livres suficientes para armazenar o arquivo
        for (int j = i; j < i + quantidade && j < TAMANHO_DISCO; j++) {
            if (disco.status[j] == BLOCO_LIVRE) {
                contiguos++;
            } else {
                break;
            }
        }
        // Caso tiver espaco, retorna o indice do primeiro bloco livre no vetor
        if (contiguos == quantidade) {
            return i;
        }
    }
    // Caso nao tenha, retorna -1
    return -1;
}

int alocarBlocos(int inode, int blocos_necessarios) {
    if (blocos_necessarios == 0) return 0;
    
    // Caso o arquivo tenha sido editado, libera os blocos ocupados anteriormente e aloca novamente
    liberarBlocos(inode);
    
    // Procura por blocos contíguos (alocacao contigua de memoria)
    int inicio = encontrarBlocosContiguos(blocos_necessarios);
    
    // Se nao encontrar espaco, retorna erro
    if (inicio == -1) {
        printf("ERRO: Nao ha espaco contiguo suficiente no disco!\n");
        printf("Blocos necessarios: %d, Blocos livres: %d\n", blocos_necessarios, disco.blocos_livres);
        return -1;
    }
    
    // Encontrando espaco, aloca os blocos, atualizando status e informacoes do bloco
    for (int i = 0; i < blocos_necessarios; i++) {
        int bloco = inicio + i;
        disco.status[bloco] = BLOCO_OCUPADO;
        disco.info[bloco].inode = inode;
        disco.info[bloco].posicao_no_arquivo = i;
        disco.blocos_livres--;
    }
    
    printf("Arquivo inode %d: %d blocos alocados (blocos %d-%d)\n", inode, blocos_necessarios, inicio, inicio + blocos_necessarios - 1);
    
    return inicio;
}

void liberarBlocos(int inode) {
    int blocos_liberados = 0;
    // Libera os blocos associados a um inode de um arquivo passando estes blocos para livres
    for (int i = 0; i < TAMANHO_DISCO; i++) {
        if (disco.info[i].inode == inode) {
            disco.status[i] = BLOCO_LIVRE;
            disco.info[i].inode = -1;
            disco.info[i].posicao_no_arquivo = -1;
            // Percorre o disco deixando os blocos livres
            for (int j = 0; j < TAMANHO_BLOCO; j++) {
                disco.dados[i][j] = 0;
            }
            disco.blocos_livres++;
            blocos_liberados++;
        }
    }
    
    if (blocos_liberados > 0) {
        printf("Arquivo inode %d: %d blocos liberados\n", inode, blocos_liberados);
    }
}

void escreverArquivoNoBlocos(int inode, const char* conteudo) {
    int tamanho = strlen(conteudo);
    int blocos_necessarios = calcularBlocosNecessarios(tamanho);
    
    // Aloca os blocos necessários dependendo do tamanho do conteudo do arquivo
    int bloco_inicial = alocarBlocos(inode, blocos_necessarios);
    if (bloco_inicial == -1) return;
    
    // Escreve o conteúdo do arquivo nos blocos
    int bytes_escritos = 0;
    for (int i = 0; i < blocos_necessarios; i++) {
        int bloco = bloco_inicial + i;
        int bytes_restantes = tamanho - bytes_escritos;
        
        // Se ainda há mais bytes que cabem no bloco, escreve tamanho completo do bloco, caso nao, escreve apenas os bytes restantes
        if (bytes_restantes >= TAMANHO_BLOCO) {
            for (int j = 0; j < TAMANHO_BLOCO; j++) {
                disco.dados[bloco][j] = conteudo[bytes_escritos + j];
            }
            bytes_escritos += TAMANHO_BLOCO;
        } else {
            for (int j = 0; j < bytes_restantes; j++) {
                disco.dados[bloco][j] = conteudo[bytes_escritos + j];
            }
            bytes_escritos += bytes_restantes;
        }
    }
    printf("Conteudo gravado: %d bytes em %d blocos\n", tamanho, blocos_necessarios);
}

void exibirEstatisticasDisco() {
    printf("\n=== ESTATISTICAS DO DISCO ===\n");
    printf("Total de blocos: %d\n", TAMANHO_DISCO);
    printf("Tamanho do bloco: %d bytes\n", TAMANHO_BLOCO);
    printf("Blocos livres: %d\n", disco.blocos_livres);
    printf("Blocos ocupados: %d\n", TAMANHO_DISCO - disco.blocos_livres);
    printf("Espaco livre: %d bytes\n", disco.blocos_livres * TAMANHO_BLOCO);
    printf("Espaco ocupado: %d bytes\n", (TAMANHO_DISCO - disco.blocos_livres) * TAMANHO_BLOCO);
    printf("=============================\n\n");
}