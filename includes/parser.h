#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"
#include "symtab.h"

typedef struct {
    Token* tokens;
    int current;
    int token_count;
    SymbolTable* current_scope;
    int next_address;
} ParserState;

ASTNode* parse(ParserState* state);

#endif // PARSER_H
