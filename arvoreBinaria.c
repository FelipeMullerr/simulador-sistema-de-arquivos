#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "arvoreBinaria.h"

int contador_inodes = 1;
NoArvore* raiz = NULL;
NoArvore* diretorio_atual = NULL;

NoArvore* criarNo(const char* nome, TipoNo tipo, int permissoes) {
    NoArvore* no = (NoArvore*) malloc(sizeof(NoArvore));
    if (!no) return NULL;

    no->tipo_no = tipo;
    no->meta.inode = contador_inodes++;
    strncpy(no->meta.nome, nome, MAX_NOME);
    no->meta.tamanho = 0;
    no->meta.tipo_dado = 0;
    time_t agora = time(NULL);
    no->meta.criado_em = agora;
    no->meta.modificado_em = agora;
    no->meta.acessado_em = agora;
    no->meta.permissoes = permissoes;

    no->pai = NULL;
    no->esquerda = NULL;
    no->direita = NULL;
    no->filhos = NULL;

    if (tipo == ARQUIVO)
        no->conteudo[0] = '\0';

    return no;
}

void adicionarNo(NoArvore* pai, NoArvore* filho) {
    if (!pai || pai->tipo_no != DIRETORIO) return;

    filho->pai = pai;

    NoArvore** atual = &pai->filhos;
    // Verifica onde inserir o novo nó (filho) na árvore, ordenando por nome do arquivo/diretório
    while (*atual) {
        if (strcmp(filho->meta.nome, (*atual)->meta.nome) < 0)
            atual = &(*atual)->esquerda;
        else
            atual = &(*atual)->direita;
    }
    // Caso seja o primeiro filho ou ache um lugar vazio, inserimos o novo filho
    *atual = filho;
}

NoArvore* buscarNo(NoArvore* diretorio, const char* nome) {
    if (!diretorio || diretorio->tipo_no != DIRETORIO) return NULL;

    NoArvore* atual = diretorio->filhos;

    // Percorre a árvore comparando o nome dos arquivos, buscando o nó com mesmo nome do nome fornecido
    // Se for encontrado, retorna o nó
    while (atual) {
        int comparacaoNomes = strcmp(nome, atual->meta.nome);
        // Se encontrou o nó, retorna 
        if (comparacaoNomes == 0) {
            return atual;
        } 
        // Caso comparacaoNomes seja < 0, significa que o nome buscado é menor que o nó atual (ordenado alfabeticamente), busca na esquerda
        else if (comparacaoNomes < 0) {
            atual = atual->esquerda;
        }
        // Caso comparacaoNomes seja > 0, significa que o nome buscado é maior que o nó atual (ordenado alfabeticamente), busca na direita
        else {
            atual = atual->direita;
        }
    }
    return NULL;
}

void removerNo(NoArvore* pai, const char* nome) {
    if (!pai || pai->tipo_no != DIRETORIO) return;

    // Busca o nó a ser removido
    NoArvore* no_remover = buscarNo(pai, nome);
    if (!no_remover) return;

    // Busca para encontra o ponteiro que aponta para o nó
    NoArvore** atual = &pai->filhos;
    while (*atual && *atual != no_remover) {
        if (strcmp(nome, (*atual)->meta.nome) < 0)
            atual = &(*atual)->esquerda;
        else
            atual = &(*atual)->direita;
    }
    if (!*atual) return;
    NoArvore* alvo = *atual;
    // Sem filhos ( é um nó folha)
    if (!alvo->esquerda && !alvo->direita) {
        *atual = NULL;
    }
    // Apenas um filho (a direita ou esquerda)
    else if (!alvo->esquerda) {
        alvo->direita->pai = alvo->pai;
        *atual = alvo->direita;
    }
    else if (!alvo->direita) {
        alvo->esquerda->pai = alvo->pai;
        *atual = alvo->esquerda;
    }
    // Dois filhos
    else {
        // Encontra o menor da subárvore direita
        NoArvore* substituto = alvo->direita;
        while (substituto->esquerda)
            substituto = substituto->esquerda;

        removerNo(alvo, substituto->meta.nome);
        return;
    }
}