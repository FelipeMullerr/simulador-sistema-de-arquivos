#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "comandos.h"
#include "blocos.h"

int main() {
    inicializarDisco();
    raiz = criarNo("/", DIRETORIO, 0755);
    diretorio_atual = raiz;

    printf("=== Sistema de Arquivos Simulado ===\n");
    printf("Digite 'help' para ver os comandos disponiveis\n");
    printf("Digite 'exit' ou 'quit' para sair\n\n");

    char linha[256];
    
    while (1) {
        exibirPrompt();
        
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\nSaindo...\n");
            break;
        }
        
        if (strlen(linha) <= 1) continue;
        
        processarComando(linha);
    }

    return 0;
}