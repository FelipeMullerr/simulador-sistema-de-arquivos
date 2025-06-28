#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "comandos.h"
#include "blocos.h"

Usuario usuario_atual = OWNER;

void cmd_mkdir(const char* nome) {
    // Busca o nó do diretorio atual, caso ja existe, não cria novamente e retorna um erro
    if (buscarNo(diretorio_atual, nome)) {
        printf("mkdir: diretorio '%s' ja existe\n", nome);
        return;
    }
    // Caso não exista, cria um novo nó com tipo DIRETORIO e adiciona este nó na Árvore
    NoArvore* novo_dir = criarNo(nome, DIRETORIO, 755);
    adicionarNo(diretorio_atual, novo_dir);
    printf("Diretorio '%s' criado.\n", nome);
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_cd(const char* nome) {
    // Caso nome seja "..", volta para o diretório pai do diretorio atual
    if (strcmp(nome, "..") == 0) {
        if (diretorio_atual->pai)
            diretorio_atual = diretorio_atual->pai;
        return;
    }
    // Caso não seja, busca o diretorio com nome digitado pelo usuário
    NoArvore* proximo = buscarNo(diretorio_atual, nome);
    // Caso o próximo não encontre ou o próximo nó seja um arquivo, retorna um erro
    if (!proximo || proximo->tipo_no != DIRETORIO) {
        printf("cd: diretorio '%s' nao encontrado\n", nome);
        return;
    }
    // Diretorio encontrado e atualiza o diretorio_atual
    diretorio_atual = proximo;
}

// ----------------------------------------------------
// ----------------------------------------------------

void exibirDiretorios(NoArvore* no) {
    // Funcao para percorrer a Árvore de diretorios e arquivos, com o metodo in-fixado (ordenado)
    if (!no) return;
    // Primeiro percorre a esquerda (valores menores) e depois percorre a direita (valores maiores)
    exibirDiretorios(no->esquerda);
    printf("%s%s\n", no->meta.nome, no->tipo_no == DIRETORIO ? "/" : "");
    exibirDiretorios(no->direita);
}

void cmd_ls() {
    exibirDiretorios(diretorio_atual->filhos);
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_rm(const char* nome) {
    NoArvore* no = buscarNo(diretorio_atual, nome);
    if(!no) {
        // Se o nó não for encontrado, retorna um erro
        printf("rm: arquivo ou diretorio '%s' nao encontrado\n", nome);
    } else if(no->tipo_no == DIRETORIO) {
        // Se o nó for um diretório, não pode ser removido com rm
        printf("rm: '%s' e um diretorio, comando rm apenas para arquivos\n", nome);
    } else {
        if(!podeAcessar(no, 'w')) {
            printf("rm: sem permissao para remover o arquivo '%s'\n", nome);
            return;
        }
        // Libera os blocos no disco que tem o mesmo inode do arquivo a ser removido
        liberarBlocos(no->meta.inode);

        // Remove o arquivo e confirma a operação
        removerNo(diretorio_atual, nome);
        printf("Arquivo '%s' removido.\n", nome);
    }
}

// ----------------------------------------------------
// ----------------------------------------------------

const char* obterTipoString(TipoDado tipo) {
    switch(tipo) {
        case CARACTERES: return "texto";
        case PROGRAMA: return "executavel";
        case CSV: return "dados";
        case DESCONHECIDO: return "desconhecido";
        default: return "indefinido";
    }
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_touch(const char* nome) {
    // Busca o diretorio atual para verificar se o arquivo já existe
    if (buscarNo(diretorio_atual, nome)) {
        printf("touch: arquivo '%s' ja existe\n", nome);
        return;
    }
    // Caso nao exista, cria o arquivo e define o tipo de dado baseado na extensao do arquivo passado
    NoArvore* novo_arquivo = criarNo(nome, ARQUIVO, 644);

    // Verifica a extensão do arquivo para definir o tipo de dados dele
    char* extensaoArq = strrchr(nome, '.');

    if(extensaoArq != NULL) {
        if(strcmp(extensaoArq, ".txt") == 0) {
            novo_arquivo->meta.tipo_dado = CARACTERES;
        } else if(strcmp(extensaoArq, ".exe") == 0) {
            novo_arquivo->meta.tipo_dado = PROGRAMA;
            novo_arquivo->meta.permissoes = 755;
        } else if(strcmp(extensaoArq, ".csv") == 0) {
            novo_arquivo->meta.tipo_dado = CSV;
        } else {
            novo_arquivo->meta.tipo_dado = DESCONHECIDO;
        }
    } else {
        novo_arquivo->meta.tipo_dado = DESCONHECIDO;
    }
    // Insere o novo arquivo no diretório atual
    adicionarNo(diretorio_atual, novo_arquivo);
    printf("Arquivo '%s' criado (tipo: %s).\n", nome, obterTipoString(novo_arquivo->meta.tipo_dado));
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_echo(const char* nome, const char* conteudo) {
    // Busca o arquivo no diretório atual
    NoArvore* arquivo = buscarNo(diretorio_atual, nome);
    
    if (!arquivo || arquivo->tipo_no != ARQUIVO) {
        printf("echo: arquivo '%s' nao encontrado\n", nome);
        return;
    }
    if(!podeAcessar(arquivo, 'w')) {
        printf("echo: sem permissao para escrever no arquivo '%s'\n", nome);
        return;
    }
    escreverArquivoNoBlocos(arquivo->meta.inode, conteudo);
    // Adiciona o conteudo ao arquivo
    strncpy(arquivo->conteudo, conteudo, MAX_CONTEUDO - 1);
    arquivo->conteudo[MAX_CONTEUDO - 1] = '\0';
    arquivo->meta.tamanho = strlen(arquivo->conteudo);
    arquivo->meta.modificado_em = time(NULL);
    arquivo->meta.acessado_em = time(NULL);
    printf("Conteudo adicionado ao arquivo '%s'.\n", nome);
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_cat(const char* nome) {
    // Busca o arquivo no diretório atual
    NoArvore* arquivo = buscarNo(diretorio_atual, nome);
    
    if (!arquivo || arquivo->tipo_no != ARQUIVO) {
        printf("cat: arquivo '%s' nao encontrado\n", nome);
        return;
    }
    if(!podeAcessar(arquivo, 'r')) {
        printf("cat: sem permissao para ler o arquivo '%s'\n", nome);
        return;
    }
    arquivo->meta.acessado_em = time(NULL);
    // Exibe o conteudo do arquivo
        printf("Conteudo de '%s':\n%s\n", nome, arquivo->conteudo);
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_cp(const char* origem, const char* destino) {
    // Busca o arquivo de origem
    NoArvore* arquivo_origem = buscarNo(diretorio_atual, origem);
    
    if (!arquivo_origem) {
        printf("cp: arquivo '%s' nao encontrado\n", origem);
        return;
    }
    if (arquivo_origem->tipo_no != ARQUIVO) {
        printf("cp: '%s' nao e um arquivo\n", origem);
        return;
    }
    if (buscarNo(diretorio_atual, destino)) {
        printf("cp: arquivo '%s' ja existe\n", destino);
        return;
    }
    // Cria um novo arquivo copia no diretorio atual com o nome de destino, depois disso é copiada as informacoes para o novo arquivo
    NoArvore* arquivo_copia = criarNo(destino, ARQUIVO, arquivo_origem->meta.permissoes);
    arquivo_copia->meta.tamanho = arquivo_origem->meta.tamanho;
    arquivo_copia->meta.tipo_dado = arquivo_origem->meta.tipo_dado;
    arquivo_copia->meta.criado_em = time(NULL); // Nova data de criação
    arquivo_copia->meta.modificado_em = arquivo_origem->meta.modificado_em;
    arquivo_copia->meta.acessado_em = arquivo_origem->meta.acessado_em;
    
    strcpy(arquivo_copia->conteudo, arquivo_origem->conteudo);
    adicionarNo(diretorio_atual, arquivo_copia);
    printf("Arquivo '%s' copiado para '%s'.\n", origem, destino);
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_mv(const char* origem, const char* destino) {
    // Busca o arquivo de origem no diretório atual
    NoArvore* arquivo_origem = buscarNo(diretorio_atual, origem);
    
    if (!arquivo_origem) {
        printf("mv: arquivo '%s' nao encontrado\n", origem);
        return;
    }
    if (arquivo_origem->tipo_no != ARQUIVO) {
        printf("mv: '%s' nao e um arquivo\n", origem);
        return;
    }
    // Se o destino comeca com / (um diretorio), remove a barra para buscar o diretorio
    const char* nome_destino = destino;
    if(destino[0] == '/') {
        nome_destino = destino + 1;
    }
    NoArvore* destino_dir = buscarNo(raiz, nome_destino);
    if (destino_dir && destino_dir->tipo_no == DIRETORIO) {
        // Verifica se já existe um arquivo com o mesmo nome no diretório destino
        if (buscarNo(destino_dir, arquivo_origem->meta.nome)) {
            printf("mv: arquivo '%s' ja existe no diretorio '%s'\n", arquivo_origem->meta.nome, destino);
            return;
        }
        // Caso nao tenha, remove do diretorio atual e adiciona no diretorio destino
        removerNo(diretorio_atual, origem);        
        adicionarNo(destino_dir, arquivo_origem);
        printf("Arquivo '%s' movido para o diretorio '%s'.\n", origem, destino);
    } else {
        // Caso destino for um arquivo, altera o nome
        if (buscarNo(diretorio_atual, nome_destino)) {
            printf("mv: arquivo '%s' ja existe\n", nome_destino);
            return;
        }
        strcpy(arquivo_origem->meta.nome, nome_destino);
        arquivo_origem->meta.modificado_em = time(NULL);
        printf("Arquivo '%s' renomeado para '%s'.\n", origem, nome_destino);
    }
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_stat(const char* nome) {
    NoArvore* arquivo = buscarNo(diretorio_atual, nome);
    if (!arquivo || arquivo->tipo_no != ARQUIVO) {
        printf("stat: arquivo '%s' nao encontrado\n", nome);
        return;
    }
    printf("=======================================\n");
    printf("Informacoes do arquivo '%s':\n", nome);
    printf("  Tamanho: %ld bytes\n", arquivo->meta.tamanho);
    printf("  Tipo de dado: %s\n", obterTipoString(arquivo->meta.tipo_dado));
    printf("  Permissoes: %d\n", arquivo->meta.permissoes);
    printf("  Criado em: %s", ctime(&arquivo->meta.criado_em));
    printf("  Acessado em: %s", ctime(&arquivo->meta.acessado_em));
    printf("  Modificado em: %s", ctime(&arquivo->meta.modificado_em));
    printf("  Acessado em: %s", ctime(&arquivo->meta.acessado_em));
    printf("=======================================\n");
}

// ----------------------------------------------------
// ----------------------------------------------------

int podeAcessar(NoArvore* arquivo, char operacao) {
    int perms_arq = arquivo->meta.permissoes;
    
    // se 000, ninguem pode acessar, se 777, todos podem acessar
    if (perms_arq == 000) return 0;
    if (perms_arq == 777) return 1;
    // Extrai o dígito da permissão para cada usuário
    int perm_owner = (perms_arq / 100) % 10;
    int perm_group = (perms_arq / 10) % 10; 
    int perm_other = perms_arq % 10;
    
    int perm_usuario;
    switch(usuario_atual) {
        case OWNER: perm_usuario = perm_owner; break;
        case GROUP: perm_usuario = perm_group; break;
        case OTHER: perm_usuario = perm_other; break;
        default: return 0;
    }
    // Verifica a permissão de acordo com a operação solicitada (R-W-X)
    // perm = 4,5,6,7 podem ler
    if (operacao == 'r') return (perm_usuario >= 4);
    // perm = 2,3,6,7 podem escrever
    if (operacao == 'w') return (perm_usuario == 2 || perm_usuario == 3 || perm_usuario == 6 || perm_usuario == 7);
    // perm = 1,3,5,7 podem executar
    if (operacao == 'x') return (perm_usuario % 2 == 1);
    
    return 0;
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_chmod(const char* permissoes, const char* arquivo) {
    NoArvore* arq = buscarNo(diretorio_atual, arquivo);
    if (!arq || arq->tipo_no != ARQUIVO) {
        printf("chmod: arquivo '%s' nao encontrado\n", arquivo);
        return;
    }
    // converte a string passada para inteiro para poder passar por parametro ao arquivo
    int perm = atoi(permissoes); 
    arq->meta.permissoes = perm;
    
    printf("Permissoes de '%s' alteradas para %s\n", arquivo, permissoes);
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_su(const char* tipo) {
    if (strcmp(tipo, "owner") == 0) {
        usuario_atual = OWNER;
        printf("Voce agora possui permissao OWNER\n");
    } else if (strcmp(tipo, "group") == 0) {
        usuario_atual = GROUP;
        printf("Voce agora possui permissao GROUP\n");
    } else if (strcmp(tipo, "other") == 0) {
        usuario_atual = OTHER;
        printf("Voce agora possui permissao OTHER\n");
    } else {
        printf("su: use 'owner', 'group' ou 'other'\n");
    }
}

// ----------------------------------------------------
// ----------------------------------------------------

void cmd_estatisticasBlocos() {
    exibirEstatisticasDisco();
}

// ----------------------------------------------------
// ----------------------------------------------------

void obterCaminhoAtual(char* caminho) {
    strcpy(caminho, "");
    
    // Se o diretorio atual for NULL, caminho é apenas "/"
    if (!diretorio_atual) {
        strcpy(caminho, "/");
        return;
    }
    // Caso o diretorio atual seja o diretorio raiz (sem pai), caminho é apenas "/"
    if (!diretorio_atual->pai) {
        strcpy(caminho, "/");
        return;
    }
    char caminhoDiretorios[500];
    NoArvore* atual = diretorio_atual;
    
    // Enquanto o diretorio que esta sendo verificado possuir pai (nao ser a raiz)
    // Continua concatenando o nome do diretorio para o caminho atual
    while (atual && atual->pai) {
        strcpy(caminhoDiretorios, "/");
        strcat(caminhoDiretorios, atual->meta.nome);
        strcat(caminhoDiretorios, caminho);
        strcpy(caminho, caminhoDiretorios);
        atual = atual->pai;
    }
    // Adiciona a raiz
    strcpy(caminhoDiretorios, "/");
    strcat(caminhoDiretorios, caminho + 1);
    strcpy(caminho, caminhoDiretorios);
}

// ----------------------------------------------------
// ----------------------------------------------------

void exibirPrompt() {
    char caminho[500];
    obterCaminhoAtual(caminho);
    printf("%s~$ ", caminho);
    fflush(stdout);
}

// ----------------------------------------------------
// ----------------------------------------------------

void processarComando(char* linha) {
    linha[strcspn(linha, "\n")] = '\0';
    
    char* comando = strtok(linha, " ");
    if (!comando) return;

    if (strcmp(comando, "ls") == 0) {
        cmd_ls();
    } else if (strcmp(comando, "mkdir") == 0) {
        char* nome = strtok(NULL, " ");
        if (nome) {
            cmd_mkdir(nome);
        } else {
            printf("mkdir: nome do diretorio obrigatorio\n");
        }
    } else if (strcmp(comando, "cd") == 0) {
        char* nome = strtok(NULL, " ");
        if (nome) {
            cmd_cd(nome);
        } else {
            printf("cd: nome do diretorio obrigatorio\n");
        }
    } else if (strcmp(comando, "touch") == 0) {
        char* nome = strtok(NULL, " ");
        if (nome) {
            cmd_touch(nome);
        } else {
            printf("touch: nome do arquivo obrigatorio\n");
        }
    } else if(strcmp(comando, "rm") == 0){
        char* nome = strtok(NULL, " ");
        if (nome) {
            cmd_rm(nome);
        } else {
            printf("rm: nome do arquivo obrigatorio\n");
        }
    } else if (strcmp(comando, "echo") == 0) {
        char* nome = strtok(NULL, " ");
        char* conteudo = strtok(NULL, "");
        if (nome && conteudo) {
            cmd_echo(nome, conteudo);
        } else {
            printf("echo: nome do arquivo e conteudo obrigatorios\n");
        }
    } else if (strcmp(comando, "cat") == 0) {
        char* nome = strtok(NULL, " ");
        if (nome) {
            cmd_cat(nome);
        } else {
            printf("cat: nome do arquivo obrigatorio\n");
        }
    } else if (strcmp(comando, "cp") == 0) {
        char* origem = strtok(NULL, " ");
        char* destino = strtok(NULL, " ");
        if (origem && destino) {
            cmd_cp(origem, destino);
        } else {
            printf("cp: origem e destino obrigatorios\n");
        }
    } else if(strcmp(comando, "mv") == 0) {
        char* origem = strtok(NULL, " ");
        char* destino = strtok(NULL, " ");
        if (origem && destino) {
            cmd_mv(origem, destino);
        } else {
            printf("mv: origem e destino obrigatorios\n");
        }
    } else if(strcmp(comando, "stat") == 0) {
        char* nome = strtok(NULL, " ");
        if (nome) {
            cmd_stat(nome);
        } else {
            printf("stat: nome do arquivo obrigatorio\n");
        }
    } else if (strcmp (comando, "chmod") == 0) {
        char* permissoes = strtok(NULL, " ");
        char* arquivo = strtok(NULL, " ");
        if (permissoes && arquivo) {
            cmd_chmod(permissoes, arquivo);
        } else {
            printf("chmod: permissoes e nome do arquivo obrigatorios\n");
        }
    } else if (strcmp(comando, "su") == 0) {
        char* tipo = strtok(NULL, " ");
        if (tipo) {
            cmd_su(tipo);
        } else {
            printf("su: tipo de usuario obrigatorio (owner, group, other)\n");
        }
    } else if(strcmp (comando, "statBlocos") == 0) {
        cmd_estatisticasBlocos();
    } else if (strcmp(comando, "exit") == 0 || strcmp(comando, "quit") == 0) {
        printf("Saindo do sistema de arquivos...\n");
        exit(0);
    } else if (strcmp(comando, "clear") == 0) {
        printf("\033[H\033[J");
    } else if (strcmp(comando, "help") == 0) {
        printf("Comandos disponiveis:\n");
        printf("  ls                        - listar arquivos e diretorios\n");
        printf("  mkdir <dir>               - criar diretorio\n");
        printf("  cd <dir>                  - mudar diretorio\n");
        printf("  touch <arq>               - criar arquivo\n");
        printf("  rm <arq>                  - remover arquivo (nao diretorio)\n");
        printf("  echo <arq> <conteudo>     - adicionar conteudo ao arquivo\n");
        printf("  cat <arq>                 - exibir conteudo do arquivo\n");
        printf("  clear                     - limpar a tela\n");
        printf("  cp <arq> <destinoCopia>   - copiar arquivo\n");
        printf("  mv <arq> <arqNovoNome>    - renomear arquivo\n");
        printf("  mv <arq> </dirDestino>    - mover arquivo para diretorio\n");
        printf("  stat <arq>                - exibir informacoes do arquivo\n");
        printf("  chmod <permissoes> <arq>  - alterar permissoes do arquivo (Ex: 644=rw- r-- r-- 755=rwx r-x r-x 700=rwx --- --- 666=rw- rw- rw- 777=rwx rwx rwx 000=--- --- ---)\n");
        printf("  su <tipo>                 - mudar usuario (owner, group, other)\n");
        printf("  statBlocos                - exibir estatisticas dos blocos do disco\n");
        printf("  help                      - mostrar esta ajuda\n");
        printf("  exit/quit                 - sair do programa\n");
    }
    else {
        printf("Comando nao reconhecido: %s (digite 'help' para ajuda)\n", comando);
    }
}