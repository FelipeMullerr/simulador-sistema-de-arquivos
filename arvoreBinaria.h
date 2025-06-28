#ifndef ARVOREBINARIA_H
#define ARVOREBINARIA_H

#include "estruturas.h"

NoArvore* criarNo(const char* nome, TipoNo tipo, int permissoes);
void adicionarNo(NoArvore* pai, NoArvore* filho);
NoArvore* buscarNo(NoArvore* diretorio, const char* nome);
void removerNo(NoArvore* pai, const char* nome);

const char* obterTipoString(TipoDado tipo);

#endif