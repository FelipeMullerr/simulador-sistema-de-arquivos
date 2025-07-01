# Sistema de Arquivos Simulado

Simulador em C de um sistema de arquivos, com diretórios em árvore, arquivos com metadados (FCB/inode), permissões RWX (owner/group/other) e simulação de blocos de dados, permite criar, navegar, manipular arquivos e diretórios, definir permissões e visualizar o estado do disco simulado, tudo via linha de comando. O projeto demonstra, de forma didática, os principais conceitos de sistemas de arquivos presentes em Sistemas Operacionais Unix/Linux.


# Organização do Código

```
sistemaArquivos_SO/
├──  main.c              # Ponto de entrada e loop principal
├──  comandos.c          # Implementação dos comandos do usuário
├──  comandos.h          # Interface dos comandos
├──  arvoreBinaria.c     # Operações da árvore de diretórios
├──  arvoreBinaria.h     # Interface da árvore
├──  blocos.c            # Simulação do sistema de blocos
├──  blocos.h            # Interface do sistema de blocos
├──  estruturas.h        # Definições das estruturas principais
├──  enums.h             # Enumerações e constantes
└──  README.md           # Este arquivo
```
---

# Instalação e Execução

## Pré-requisitos

- **Compilador GCC** (versão 7.0 ou superior)
- **Sistema Operacional**: Linux ou macOS ou Windows (WSL)
- **Terminal/Prompt de Comando**

## Compilação

```bash
# Clone o repositório
git clone https://github.com/FelipeMullerr/simulador-sistema-de-arquivos.git
cd sistemaArquivos_SO

# Compilação usando GCC
gcc main.c comandos.c arvoreBinaria.c blocos.c -o sistemaArquivos

# Ou usando um Makefile (se disponível)
make
```

### Execução

```bash
# Execute o simulador
./sistemaArquivos

```
---

# Escolhas de Design

## Árvore Binária para Diretórios
Para estruturar a hierarquia de arquivos e diretórios, foi escolhida a implementação de uma árvore binária de busca, devido a permite organizar os arquivos e subdiretórios de forma ordenada alfabeticamente, facilitando operações como listagem, busca e inserção de novos elementos sem a necessidade de reordenar listas.
O uso de ponteiros para conectar cada nó ao seu pai e aos filhos esquerdo e direito reflete a maneira como o sistema mantém relações entre arquivos e diretórios, além de simplificar comandos como `cd`, `cd ..`, `mkdir` e `touch`, já que a navegação entre níveis da árvore se torna direta e eficiente.
Essa estrutura também permite que caminhos sejam facilmente construídos e percorridos, tornando o gerenciamento do sistema de arquivos mais intuitivo.

## FCB (File Control Block)

A representação dos arquivos e diretórios foi feita por meio da integração entre o File Control Block (FCB), implementado na struct `Metadados`, e a struct `NoArvore`, que representa cada elemento da árvore.
O FCB armazena todos os metadados relevantes, como nome, inode, tamanho, tipo, permissões e timestamps, enquanto a struct `NoArvore` incorpora esses metadados e adiciona os ponteiros necessários para a navegação na árvore, além do campo para o conteúdo dos arquivos.
Essa composição garante que cada arquivo ou diretório tenha seus atributos acessados de forma eficiente durante as operações do sistema, mantendo a integridade da hierarquia e facilitando a implementação de comandos que dependem desses atributos.

---

# Conceitos Teóricos Implementados

## 1. Atributos de Arquivo, FCB e inode Simulado

Cada arquivo possui atributos completos definidos no FCB através da struct `Metadados`:

```c
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
```
**Inode Simulado**: Integração do FCB com estrutura hierárquica de diretórios utilizada:
O inode é implementado através da integração `Metadados` + `NoArvore`, conectando atributos do arquivo à estrutura da árvore:

```c
typedef struct NoArvore {
    Metadados meta;
    TipoNo tipo_no;
    char conteudo[MAX_CONTEUDO];
    
    struct NoArvore* pai;
    struct NoArvore* esquerda;
    struct NoArvore* direita;
} NoArvore;
```

Cada arquivo recebe um inode único incremental no momento da criação, simulando o comportamento real de sistemas de arquivos Unix.

## 2. Árvore de Diretórios

**Implementação:** Utiliza uma árvore binária de busca para representar a estrutura de diretórios e arquivos, garantindo que todos os elementos sejam organizados automaticamente em ordem alfabética. Essa estrutura facilita a navegação, a busca eficiente e a manutenção do sistema de arquivos.

**Operações implementadas**:
- **Inserção**: `adicionarNo()` mantém ordem alfabética na inserção
- **Busca**: `buscarNo()` localiza arquivos/diretórios pelo seus nomes.
- **Remoção**: `removerNo()` preserva estrutura de diretórios e arquivos da árvore após remoção de algum arquivo pelo comando `rm`.

## 3. Controle de Acesso e Permissões RWX

O sistema implementa controle de acesso baseado no modelo Unix com três níveis de usuário (owner/group/other) e três tipos de permissão (read/write/execute).

### Funcionamento do Sistema de Permissões

**Estrutura das Permissões**: Cada arquivo possui um valor de 3 dígitos (ex: 755, 644) onde:
- **Primeiro dígito**: Permissões do proprietário (owner)
- **Segundo dígito**: Permissões do grupo (group)  
- **Terceiro dígito**: Permissões de outros usuários (other)

**Verificação por Bitmasks**: A função `podeAcessar()` utiliza operações matemáticas para extrair e verificar permissões:
```c
int podeAcessar(NoArvore* arquivo, char operacao) {
    int perms = arquivo->meta.permissoes;
    
    // Extrai permissões por categoria usando divisão
    int perm_owner = (perms / 100) % 10;
    int perm_group = (perms / 10) % 10;
    int perm_other = perms % 10;
    
    // Seleciona permissão do usuário atual
    int perm_usuario;
    switch(usuario_atual) {
        case OWNER: perm_usuario = perm_owner; break;
        case GROUP: perm_usuario = perm_group; break;  
        case OTHER: perm_usuario = perm_other; break;
    }
    
    // Verifica operação usando bitmasks
    if (operacao == 'r') return (perm_usuario >= 4);  // 100 (bit read)
    if (operacao == 'w') return (perm_usuario & 2);   // 010 (bit write)
    if (operacao == 'x') return (perm_usuario & 1);   // 001 (bit execute)
}
```
### Funcionamento do chmod

O comando `chmod` implementa o sistema de permissões Unix através da conversão de valores octais em permissões binárias e aplicação de bitmasks para verificação de acesso.

**Sistema de Numeração para Permissões**:
Cada dígito (0-7) representa uma combinação de três bits, correspondendo às permissões Read, Write, Execute:

| Digito | Binário | Permissões | Significado |
|-------|---------|------------|-------------|
| `0` | `000`| `---` | Nenhuma permissão |
| `1` | `001`| `--x` | Apenas execução |
| `2` | `010`| `-w-` | Apenas escrita |
| `3` | `011`| `-wx` | Escrita + execução |
| `4` | `100`| `r--` | Apenas leitura |
| `5` | `101`| `r-x` | Leitura + execução |
| `6` | `110`| `rw-` | Leitura + escrita |
| `7` | `111`| `rwx` | Todas as permissões |

**Estrutura de Permissões de 3 Dígitos**: (`Exemplo: valor 755`)
- Owner: `7` → `rwx` (pode ler, escrever e executar)
- Group: `5` → `r-x` (pode ler e executar, mas não escrever)
- Other: `5` → `r-x` (pode ler e executar, mas não escrever)

**Implementação do chmod**:
```c
void cmd_chmod(const char* permissoes, const char* arquivo) {
    NoArvore* arq = buscarNo(diretorio_atual, arquivo);
    int perm = atoi(permissoes);
    arq->meta.permissoes = perm; 
}
```
### Exemplos de Permissões

| Código | Binário | Significado | Uso Típico |
|--------|---------|-------------|------------|
| `755` | `rwxr-xr-x` | Owner: total, Group/Other: ler/executar | Executáveis |
| `644` | `rw-r--r--` | Owener: ler/escrever, Group/Other: ler | Documentos |
| `700` | `rwx------` | Apenas Owner acessa | Arquivos privados |
| `666` | `rw-rw-rw-` | Todos podem ler/escrever | Arquivos compartilhados |

**Verificação de Permissões**: Antes de cada operação (ler, escrever, executar), ou seja, operações como `echo`, `rm`, `cat`, o sistema:
1. Identifica o usuário atual (`usuario_atual`);
2. Extrai as permissões correspondentes do arquivo;
3. Compara com a operação solicitada usando bitmasks usando a função `podeAcessar()`;
4. Permite ou nega a operação com mensagem de erro apropriada para cada caso.

---

## 4. **Simulação de Alocação de Blocos**

### Alocação Contígua Implementada

O projeto implementa um sistema de blocos e disco com **alocação contígua** com as seguintes características:

```c
int alocarBlocos(int inode, int blocos_necessarios) {
    // Liberação de blocos anteriores (reedição)
    liberarBlocos(inode);
    
    // Busca por blocos contíguos livres no disco
    int inicio = encontrarBlocosContiguos(blocos_necessarios);
    
    // Alocação sequencial
    for (int i = 0; i < blocos_necessarios; i++) {
        disco.status[inicio + i] = BLOCO_OCUPADO;
        disco.info[inicio + i].inode = inode;
        disco.info[inicio + i].posicao_no_arquivo = i;
    }
}
```

### Vantagens e Desvantagens Demonstradas
**Vantagens:**
- **Performance**: Acesso sequencial otimizado
- **Simplicidade**: Implementação direta

**Desvantagens:**
- **Fragmentação externa**: Espaços pequenos inutilizados
- **Realocação**: Necessário realocação quando arquivo cresce de tamanho (é editado)

---

# Comandos Disponíveis

| Comando | Função | Equivalente Linux |
|---------|--------|-------------------|
| `ls` | Lista conteúdo do diretório atual | `ls` |
| `mkdir <nome>` | Cria novo diretório | `mkdir` |
| `cd <nome>` | Navega para diretório especificado | `cd` |
| `cd ..` | Volta ao diretório pai | `cd ..` |
| `touch <arquivo>` | Cria arquivo vazio | `touch` |
| `echo <arquivo> <texto>` | Escreve conteúdo no arquivo | `echo "texto" > arquivo` |
| `cat <arquivo>` | Exibe conteúdo do arquivo | `cat` |
| `rm <nome>` | Remove arquivo ou diretório | `rm / rmdir` |
| `cp <origem> <destino>` | Copia arquivo | `cp` |
| `mv <origem> <destino>` | Move/renomeia arquivo | `mv` |
| `stat <nome>` | Exibe metadados completos (FCB) | `stat` |
| `chmod <perm> <arquivo>` | Altera permissões (formato octal) | `chmod` |
| `su <owner/group/other>` | Muda contexto de usuário | `su` |
| `statBlocos` | Mostra estatísticas do disco simulado | `df -h` |

---

## Exemplos de Uso

### Operações Básicas vs Linux
```bash
# Simulador                          # Linux equivalente
/~$ mkdir projetos                   $ mkdir projetos
/~$ cd projetos                      $ cd projetos  
/projetos~$ touch README.md          $ touch README.md
/projetos~$ echo README.md "# Docs"  $ echo "# Docs" > README.md
/projetos~$ chmod 644 README.md      $ chmod 644 README.md
/projetos~$ stat README.md           $ stat README.md
/projetos~$ cat README.md            $ cat README.md
/projetos~$ ls                       $ ls
/projetos~$ statBlocos               $ df -h
```

### Demonstração de Permissões
```bash
/~$ touch secreto.txt
/~$ chmod 600 secreto.txt    # rw-------
/~$ su other                 # Muda para usuário 'other'
/~$ cat secreto.txt          # ERRO: Permissão negada
/~$ su owner                 # Volta para 'owner'
/~$ cat secreto.txt          # OK: Acesso permitido
```
---

# Projeto desenvolvido por

- **Nome(s):** Felipe Muller Schvuchov e Kaua Betineli Correa
- **Universidade:** Universidade do Vale do Itajaí - Univali
- **Curso:** Ciência da Computação
- **Disciplina:** Sistemas Operacionais
- **Professor:** Michael D C Alves
- **Período:** 5º Período

---
