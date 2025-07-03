#ifndef LEXER_H
#define LEXER_H

#include "token.h"

Token* tokenize(const char* source_code, int* token_count);

void free_tokens(Token* tokens, int token_count);

#endif // LEXER_H

