# Projeto de Compilador

Este projeto é um compilador para uma linguagem de programação simples, construído como parte de um estudo sobre a teoria e a prática de compiladores. O programa implementa as fases de análise léxica, sintática e semântica.

## Funcionalidades

-   **Analisador Léxico:** Converte o código-fonte em uma sequência de tokens.
-   **Analisador Sintático (Parser):** Constrói uma Árvore Sintática Abstrata (AST) a partir dos tokens, validando a estrutura do código.
-   **Analisador Semântico:** Utiliza uma Tabela de Símbolos para verificar a consistência semântica do código, como a declaração de variáveis e o escopo.

## Estrutura do Projeto

-   `main.c`: O ponto de entrada do compilador. Orquestra as fases de análise.
-   `Makefile`: Script de construção para compilar o projeto.
-   `includes/`: Contém os arquivos de cabeçalho (`.h`) com as definições das estruturas de dados (Tokens, AST, Tabela de Símbolos) e as assinaturas das funções.
-   `src/`: Contém a implementação (`.c`) dos módulos do compilador (lexer, parser, tabela de símbolos, etc.).
-   `APRESENTACAO.md`: Um roteiro detalhado que explica o design e a implementação de cada parte do projeto.

## Como Compilar e Executar

### Pré-requisitos

-   GCC (GNU Compiler Collection)
-   Make

### Compilação

Para compilar o projeto, execute o seguinte comando no terminal:

```bash
make
```

Isso irá gerar um executável chamado `analisador`.

### Execução

Para analisar um arquivo de código-fonte, use o seguinte comando:

```bash
./analisador caminho/para/seu/arquivo.lang
```

O compilador irá processar o arquivo e imprimir as seguintes informações:

1.  A lista de tokens gerada pelo analisador léxico.
2.  O resultado da análise sintática e semântica.
3.  O conteúdo da Tabela de Símbolos após a análise.
4.  A representação da Árvore Sintática Abstrata (AST).

### Limpeza

Para remover os arquivos gerados pela compilação, execute:

```bash
make clean
```

