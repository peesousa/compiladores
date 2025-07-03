#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    // início e fim de bloco de código
    TOKEN_BEGIN,
    TOKEN_END,

    // tipos de dados
    TOKEN_INT,
    TOKEN_FLOAT,

    // condicionais e laços
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ENDIF,
    TOKEN_ELSE,
    TOKEN_ENDELSE,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_ENDWHILE,

    // saída de dados
    TOKEN_PRINT,
    TOKEN_SCAN,

    // funções e procedimentos.
    TOKEN_FUNCTION,
    TOKEN_PROCEDURE,
    TOKEN_RETURN,

    // símbolos de pontuação que estruturam o código.
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,

    // operadores aritmeticos
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_SLASH,

    // atribuição e comparação
    TOKEN_ASSIGN,
    TOKEN_EQ,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_NEQ,

    // nomes de variáveis e funções criados
    TOKEN_IDENTIFIER,

    TOKEN_INTEGER_LITERAL,
    TOKEN_FLOAT_LITERAL,

    // fim do arquivo.
    TOKEN_EOF,

    // palavra desconhecida
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;
    int line;
} Token;

const char* token_type_to_string(TokenType type);

#endif // TOKEN_H
