#ifndef COMANDOS_H
#define COMANDOS_H

#include "arvoreBinaria.h"

extern Usuario usuario_atual;

void cmd_mkdir(const char* nome);
void cmd_cd(const char* nome);
void cmd_ls();
void cmd_rm(const char* nome);
void cmd_touch(const char* nome);
void cmd_echo(const char* nome, const char* conteudo);
void cmd_cat(const char* nome);
void cmd_cp(const char* origem, const char* destino);
void cmd_mv(const char* origem, const char* destino);
void cmd_stat(const char* nome);
int podeAcessar(NoArvore* arquivo, char operacao);
void cmd_chmod(const char* permissoes, const char* arquivo);
void cmd_su(const char* tipo);

void exibirDiretorios(NoArvore* no);
void obterCaminhoAtual(char* caminho);
void exibirPrompt();
void processarComando(char* linha);

#endif