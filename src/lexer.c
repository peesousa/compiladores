#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_TOKEN_CAPACITY 50

typedef struct { const char* word; TokenType type; } Keyword;

static Keyword keywords[] = {
    {"begin", TOKEN_BEGIN}, {"end", TOKEN_END}, {"int", TOKEN_INT},
    {"float", TOKEN_FLOAT}, {"if", TOKEN_IF}, {"then", TOKEN_THEN},
    {"endif", TOKEN_ENDIF}, {"else", TOKEN_ELSE}, {"endelse", TOKEN_ENDELSE},
    {"while", TOKEN_WHILE}, {"do", TOKEN_DO}, {"endwhile", TOKEN_ENDWHILE},
    {"print", TOKEN_PRINT}, {"scan", TOKEN_SCAN}, {"function", TOKEN_FUNCTION},
    {"procedure", TOKEN_PROCEDURE}, {"return", TOKEN_RETURN},
    {NULL, TOKEN_UNKNOWN}
};

typedef struct {
    const char* start;
    const char* current;
    int line;
    Token* tokens;
    int token_count;
    int token_capacity;
} LexerState;

static void add_token(LexerState* state, TokenType type, const char* lexeme, int length) {
    if (state->token_count >= state->token_capacity) {
        state->token_capacity *= 2;
        state->tokens = (Token*)realloc(state->tokens, state->token_capacity * sizeof(Token));
    }
    Token* token = &state->tokens[state->token_count++];
    token->type = type;
    token->line = state->line;
    token->lexeme = (char*)malloc(length + 1);
    strncpy(token->lexeme, lexeme, length);
    token->lexeme[length] = '\0';
}

static void number(LexerState* state) {
    while (isdigit(*state->current)) state->current++;
    TokenType type = TOKEN_INTEGER_LITERAL;
    if (*state->current == '.' && isdigit(state->current[1])) {
        type = TOKEN_FLOAT_LITERAL;
        state->current++;
        while (isdigit(*state->current)) state->current++;
    }
    add_token(state, type, state->start, state->current - state->start);
}

static void identifier(LexerState* state) {
    while (isalnum(*state->current) || *state->current == '_') state->current++;
    int length = state->current - state->start;
    TokenType type = TOKEN_IDENTIFIER;
    for (int i = 0; keywords[i].word != NULL; i++) {
        if (strncmp(state->start, keywords[i].word, length) == 0 && keywords[i].word[length] == '\0') {
            type = keywords[i].type;
            break;
        }
    }
    add_token(state, type, state->start, length);
}

Token* tokenize(const char* source_code, int* token_count) {
    LexerState state_obj = { source_code, source_code, 1, (Token*)malloc(INITIAL_TOKEN_CAPACITY * sizeof(Token)), 0, INITIAL_TOKEN_CAPACITY };
    LexerState* state = &state_obj;

    while (*state->current != '\0') {
        state->start = state->current;
        char c = *state->current++;
        switch (c) {
            case ' ': case '\r': case '\t': break;
            case '\n': state->line++; break;
            case '(': add_token(state, TOKEN_LPAREN, state->start, 1); break;
            case ')': add_token(state, TOKEN_RPAREN, state->start, 1); break;
            case ',': add_token(state, TOKEN_COMMA, state->start, 1); break;
            case ';': add_token(state, TOKEN_SEMICOLON, state->start, 1); break;
            case '+': add_token(state, TOKEN_PLUS, state->start, 1); break;
            case '-': add_token(state, TOKEN_MINUS, state->start, 1); break;
            case '*': add_token(state, TOKEN_ASTERISK, state->start, 1); break;
            
            case '/':
                if (*state->current == '/') {
                    while (*state->current != '\n' && *state->current != '\0') {
                        state->current++;
                    }
                } else {
                    add_token(state, TOKEN_SLASH, state->start, 1);
                }
                break;
            
            case '=':
                if (*state->current == '=') {
                    state->current++;
                    add_token(state, TOKEN_EQ, state->start, 2);
                } else {
                    add_token(state, TOKEN_ASSIGN, state->start, 1);
                }
                break;
            case '!':
                if (*state->current == '=') {
                    state->current++;
                    add_token(state, TOKEN_NEQ, state->start, 2);
                } else {
                    add_token(state, TOKEN_UNKNOWN, state->start, 1);
                }
                break;
            case '<':
                if (*state->current == '=') {
                    state->current++;
                    add_token(state, TOKEN_LTE, state->start, 2);
                } else {
                    add_token(state, TOKEN_LT, state->start, 1);
                }
                break;
            case '>':
                if (*state->current == '=') {
                    state->current++;
                    add_token(state, TOKEN_GTE, state->start, 2);
                } else {
                    add_token(state, TOKEN_GT, state->start, 1);
                }
                break;

            default:
                if (isalpha(c) || c == '_') {
                    state->current--;
                    identifier(state);
                } else if (isdigit(c)) {
                    state->current--;
                    number(state);
                } else {
                    add_token(state, TOKEN_UNKNOWN, state->start, 1);
                }
                break;
        }
    }
    add_token(state, TOKEN_EOF, "EOF", 3);
    *token_count = state->token_count;
    return state->tokens;
}

void free_tokens(Token* tokens, int token_count) {
    if (!tokens) return;
    for (int i = 0; i < token_count; i++) {
        free(tokens[i].lexeme);
    }
    free(tokens);
}
