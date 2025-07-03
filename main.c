#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

static char* read_file(const char* path) {
  FILE* file = fopen(path, "rb");
  if (file == NULL) {
      fprintf(stderr, "Could not open file \"%s\".\n", path);
      exit(74);
  }
  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);
  char* buffer = (char*)malloc(file_size + 1);
  if (buffer == NULL) {
      fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
      exit(74);
  }
  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size) {
      fprintf(stderr, "Could not read file \"%s\".\n", path);
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

    char* source_code = read_file(argv[1]);
    
    int token_count = 0;
    Token* tokens = tokenize(source_code, &token_count);

    printf("--- Tokens ---\n");    for (int i = 0; i < token_count; i++) {        printf("  [%d] Type: %-20s Lexeme: '%s'\n", tokens[i].line, token_type_to_string(tokens[i].type), tokens[i].lexeme);    }    printf("\n--- Análise Sintática e Semântica ---\n");        SymbolTable* global_scope = scope_create();    ParserState state = {tokens, 0, token_count, global_scope, 0};        ASTNode* ast_root = parse(&state);    printf("Análise concluída com sucesso.\n");    printf("\n--- Tabela de Símbolos ---\n");    scope_dump(global_scope);        printf("\n--- Árvore Sintática Abstrata ---\n");    print_ast(ast_root, 0);    printf("\nLimpando memória...\n");
    free(source_code);
    free_tokens(tokens, token_count);
    free_ast(ast_root);
    printf("Concluído.\n");

    return 0;
}
