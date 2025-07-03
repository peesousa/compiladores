#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

// ler o arquivo de codigo fonte
static char* read_file(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
      fprintf(stderr, "Não foi possível abrir o arquivo \"%s\".\n", path);
      exit(74);
  }

  // descobre o tamanho do arquivo
  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);
  // aloca memória para guardar o conteúdo do arquivo
  char* buffer = (char*)malloc(file_size + 1);
  if (buffer == NULL) {
      fprintf(stderr, "Memória insuficiente para ler \"%s\".\n", path);
      exit(74);
  }
  // lê o arquivo para o buffer
  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size) {
      fprintf(stderr, "Não foi possível ler o arquivo \"%s\".\n", path);
      exit(74);
  }
  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <ficheiro.lang>\n", argv[0]);
        return 1;
    }

    // lê o código-fonte do arquivo
    char* source_code = read_file(argv[1]);
    
    // análise léxica
    int token_count = 0;
    Token* tokens = tokenize(source_code, &token_count);

    printf("--- Tokens ---\n");
    // imprime os tokens
    for (int i = 0; i < token_count; i++) {
      printf("  [%d] Tipo: %-20s Lexema: '%s'\n", tokens[i].line, token_type_to_string(tokens[i].type), tokens[i].lexeme);
    }
    printf("\n--- Análise Sintática e Semântica ---\n");
    
    // análise sintática e semântica (parser)
    // pega os tokens e constrói a AST e verifica se o código faz sentido erros semânticos
    SymbolTable* global_scope = scope_create();
    ParserState state = {tokens, 0, token_count, global_scope, 0};
    ASTNode* ast_root = parse(&state);
    printf("Análise concluída com sucesso.\n");
    
    printf("\n--- Tabela de Símbolos ---\n");
    scope_dump(global_scope);
    
    printf("\n--- Árvore Sintática Abstrata ---\n");
    print_ast(ast_root, 0);
    
    printf("\nLimpando memória...\n");
    free(source_code);
    free_tokens(tokens, token_count);
    free_ast(ast_root);
    printf("Concluído.\n");

    return 0;
}
