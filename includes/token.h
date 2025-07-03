#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    // inicio e fim de um bloco de codigo
    TOKEN_BEGIN,
    TOKEN_END,

    // tipos de dados primitivos
    TOKEN_INT,
    TOKEN_FLOAT,

    // estruturas de decisao e repeticao
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ENDIF,
    TOKEN_ELSE,
    TOKEN_ENDELSE,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_ENDWHILE,

    // entrada e saida
    TOKEN_PRINT,
    TOKEN_SCAN,

    // funcoes e procedimentos
    TOKEN_FUNCTION,
    TOKEN_PROCEDURE,
    TOKEN_RETURN,

    // pontuacao
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_COMMA,    
    TOKEN_SEMICOLON,

    // operadores aritméticos
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_SLASH,

    // atribuicao e comparacao
    TOKEN_ASSIGN,
    TOKEN_EQ,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_NEQ,

    // definicao de variavel e funcoes
    TOKEN_IDENTIFIER,

    // valores fixos
    TOKEN_INTEGER_LITERAL,
    TOKEN_FLOAT_LITERAL,

    // final do arquivo
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
