# Compilador IFB

Este é um projeto de compilador desenvolvido como parte da disciplina de Compiladores do IFB. Atualmente, o projeto está em desenvolvimento, mas já possui funcionalidades básicas para análise léxica, sintática e semântica de uma linguagem de programação simplificada.

## Estrutura do Projeto

- `main.c`: Ponto de entrada principal do compilador.
- `includes/`: Contém os arquivos de cabeçalho (`.h`) para as diferentes partes do compilador (AST, lexer, parser, tabela de símbolos, tokens).
- `src/`: Contém os arquivos de implementação (`.c`) para as diferentes partes do compilador.
- `testes/`: Contém exemplos de código na linguagem a ser compilada (`.lang`) para testes.

## Como Compilar

Para compilar o projeto, navegue até o diretório raiz do projeto e execute o comando `make`:

```bash
make
```

Isso irá gerar um executável chamado `compilador` no diretório raiz do projeto.

## Como Executar

Após a compilação, você pode executar o compilador passando um arquivo `.lang` como entrada. Por exemplo:

```bash
./compilador testes/aritmetica.lang
```

**Nota:** O projeto ainda está em desenvolvimento, e algumas funcionalidades podem não estar completas ou podem conter erros.