#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

// cada função cuida de uma parte da gramática
static ASTNode* parse_Statement(ParserState* state);
static ASTNode* parse_Expression(ParserState* state);
static ASTNode* parse_ProgramBlock(ParserState* state);
static ASTNode* parse_FunctionDeclaration(ParserState* state);
static ASTNode* parse_Conditional(ParserState* state);
static ASTNode* parse_Loop(ParserState* state);
static ASTNode* parse_Factor(ParserState* state);
static ASTNode* parse_Term(ParserState* state);
static SymbolDataType parse_Type(ParserState* state);

// olha o token atual
static Token peek(ParserState* state) {
  return state->tokens[state->current];
}

// avança para o próximo token
static void advance(ParserState* state) {
  if (state->current < state->token_count) state->current++;
}

// consome o token atual
static Token consume(ParserState* state, TokenType type, const char* message) {
    if (peek(state).type == type) {
        Token t = peek(state); advance(state); return t;
    }
    fprintf(stderr, "Erro de Sintaxe na linha %d: %s. Encontrado '%s' (%s) ao invés.\n", peek(state).line, message, peek(state).lexeme, token_type_to_string(peek(state).type));
    exit(EXIT_FAILURE);
}

// checa se o token atual é de um tipo específico sem consumir ele
static int check(ParserState* state, TokenType type) { return peek(state).type == type; }

// analisa um tipo de dado int ou float
static SymbolDataType parse_Type(ParserState* state) {
    if (check(state, TOKEN_INT)) { consume(state, TOKEN_INT, ""); return TYPE_INTEGER; }
    if (check(state, TOKEN_FLOAT)) { consume(state, TOKEN_FLOAT, ""); return TYPE_FLOAT; }
    fprintf(stderr, "Erro de Sintaxe: Esperado um tipo (int ou float).\n"); exit(EXIT_FAILURE);
}

// analisa a lista de argumentos de uma chamada de função.
static ASTNode* parse_ArgumentList(ParserState* state) {
    ASTNode* args = create_node(NODE_ARG_LIST, NULL);
    consume(state, TOKEN_LPAREN, "Esperado '('.");
    if (!check(state, TOKEN_RPAREN)) {
        do { 
            add_child(args, parse_Expression(state));
        } while (check(state, TOKEN_COMMA) && (consume(state, TOKEN_COMMA, ""), 1));
    }
    consume(state, TOKEN_RPAREN, "Esperado ')'.");
    return args;
}

// analisa os fatores de uma expressão, identificadores, literais (números) ou expressões entre parênteses
static ASTNode* parse_Factor(ParserState* state) {
    if (check(state, TOKEN_IDENTIFIER) && state->tokens[state->current + 1].type == TOKEN_LPAREN) {
        Token id = consume(state, TOKEN_IDENTIFIER, "");
        SymbolNode* sym = scope_lookup(state->current_scope, id.lexeme);
        if (!sym || (sym->kind != KIND_FUNCTION && sym->kind != KIND_PROCEDURE)) {
            fprintf(stderr, "Erro Semântico: '%s' não é uma função ou procedimento.\n", id.lexeme); exit(EXIT_FAILURE);
        }
        ASTNode* call = create_node(NODE_FUNC_CALL, id.lexeme);
        add_child(call, parse_ArgumentList(state));
        return call;
    }
    if (check(state, TOKEN_IDENTIFIER)) {
        Token id = consume(state, TOKEN_IDENTIFIER, "");
        if (scope_lookup(state->current_scope, id.lexeme) == NULL) {
            fprintf(stderr, "Erro Semântico: Variável '%s' não declarada.\n", id.lexeme); exit(EXIT_FAILURE);
        }
        return create_node(NODE_IDENTIFIER, id.lexeme);
    }
    if (check(state, TOKEN_INTEGER_LITERAL)) return create_node(NODE_INT_LITERAL, consume(state, TOKEN_INTEGER_LITERAL, "").lexeme);
    if (check(state, TOKEN_FLOAT_LITERAL)) return create_node(NODE_FLOAT_LITERAL, consume(state, TOKEN_FLOAT_LITERAL, "").lexeme);
    if (check(state, TOKEN_LPAREN)) {
        consume(state, TOKEN_LPAREN, "");
        ASTNode* expr = parse_Expression(state);
        consume(state, TOKEN_RPAREN, "");
        return expr;
    }
    fprintf(stderr, "Erro de Sintaxe: Token inesperado na expressão.\n"); exit(EXIT_FAILURE);
}

// analisa os termos de uma expressão
static ASTNode* parse_Term(ParserState* state) {
    if (check(state, TOKEN_MINUS)) {
        consume(state, TOKEN_MINUS, "");
        ASTNode* operand = parse_Factor(state);
        ASTNode* negate_node = create_node(NODE_NEGATE, NULL);
        add_child(negate_node, operand);
        return negate_node;
    }
    ASTNode* node = parse_Factor(state);
    while (check(state, TOKEN_ASTERISK) || check(state, TOKEN_SLASH)) {
        Token op = consume(state, peek(state).type, "");
        ASTNode* right = parse_Factor(state);
        ASTNode* new_node = create_node((op.type == TOKEN_ASTERISK) ? NODE_MUL : NODE_DIV, NULL);
        add_child(new_node, node); add_child(new_node, right);
        node = new_node;
    }
    return node;
}

// analisa uma expressão completa
static ASTNode* parse_Expression(ParserState* state) {
    ASTNode* node = parse_Term(state);
    while (check(state, TOKEN_PLUS) || check(state, TOKEN_MINUS) || check(state, TOKEN_EQ) || check(state, TOKEN_NEQ) || check(state, TOKEN_LT) || check(state, TOKEN_LTE) || check(state, TOKEN_GT) || check(state, TOKEN_GTE)) {
        Token op = consume(state, peek(state).type, "");
        ASTNode* right = parse_Term(state);
        NodeType type;
        switch(op.type) {
            case TOKEN_PLUS: type = NODE_ADD; break;
            case TOKEN_MINUS: type = NODE_SUB; break;
            case TOKEN_EQ: type = NODE_EQ; break;
            case TOKEN_NEQ: type = NODE_NEQ; break;
            case TOKEN_LT: type = NODE_LT; break;
            case TOKEN_LTE: type = NODE_LTE; break;
            case TOKEN_GT: type = NODE_GT; break;
            case TOKEN_GTE: type = NODE_GTE; break;
            default: type = -1;
        }
        ASTNode* new_node = create_node(type, NULL);
        add_child(new_node, node); add_child(new_node, right);
        node = new_node;
    }
    return node;
}

// analisa a declaração de uma variável
static ASTNode* parse_Declaration(ParserState* state) {
    SymbolDataType type = parse_Type(state);
    Token id = consume(state, TOKEN_IDENTIFIER, "Esperado um identificador.");
    scope_insert(state->current_scope, id.lexeme, KIND_VARIABLE, type, id.line, state->next_address++);
    ASTNode* decl_node = create_node(NODE_DECLARATION, id.lexeme);

    if (check(state, TOKEN_ASSIGN)) {
        consume(state, TOKEN_ASSIGN, "Esperado '='.");
        ASTNode* expr = parse_Expression(state);
        ASTNode* assign_node = create_node(NODE_ASSIGNMENT, id.lexeme);
        add_child(assign_node, expr);

        ASTNode* decl_assign_node = create_node(NODE_DECL_ASSIGN, NULL);
        add_child(decl_assign_node, decl_node);
        add_child(decl_assign_node, assign_node);
        return decl_assign_node;
    }

    return decl_node;
}

// analisa uma instrução de retorno
static ASTNode* parse_ReturnStatement(ParserState* state) {
    consume(state, TOKEN_RETURN, "Esperado 'return'.");
    ASTNode* ret = create_node(NODE_RETURN_STMT, NULL);
    add_child(ret, parse_Expression(state));
    consume(state, TOKEN_SEMICOLON, "Esperado ';'.");
    return ret;
}

// analisa uma estrutura condicional
static ASTNode* parse_Conditional(ParserState* state) {
    consume(state, TOKEN_IF, "Esperado 'if'.");
    ASTNode* cond_node = create_node(NODE_CONDITIONAL, NULL);
    add_child(cond_node, parse_Expression(state));
    consume(state, TOKEN_THEN, "Esperado 'then'.");
    add_child(cond_node, parse_ProgramBlock(state));

    if (check(state, TOKEN_ELSE)) {
        consume(state, TOKEN_ELSE, "");
        add_child(cond_node, parse_ProgramBlock(state));
        consume(state, TOKEN_ENDELSE, "Esperado 'endelse'.");
    }
    consume(state, TOKEN_ENDIF, "Esperado 'endif'.");
    return cond_node;
}

// analisa um laço de repetição
static ASTNode* parse_Loop(ParserState* state) {
    consume(state, TOKEN_WHILE, "Esperado 'while'.");
    ASTNode* loop_node = create_node(NODE_LOOP, NULL);
    add_child(loop_node, parse_Expression(state));
    consume(state, TOKEN_DO, "Esperado 'do'.");
    add_child(loop_node, parse_ProgramBlock(state));
    consume(state, TOKEN_ENDWHILE, "Esperado 'endwhile'.");
    return loop_node;
}

// olha o token atual e decide qual tipo de instrução está vindo
static ASTNode* parse_Statement(ParserState* state) {
    if (check(state, TOKEN_IF)) return parse_Conditional(state);
    if (check(state, TOKEN_WHILE)) return parse_Loop(state);
    if (check(state, TOKEN_RETURN)) return parse_ReturnStatement(state);
    if (check(state, TOKEN_PRINT)) {
        consume(state, TOKEN_PRINT, "");
        ASTNode* print_node = create_node(NODE_PRINT, NULL);
        add_child(print_node, parse_ArgumentList(state)); // o que vai ser impresso.
        consume(state, TOKEN_SEMICOLON, "Esperado ';' após a instrução print.");
        return print_node;
    }
    if (check(state, TOKEN_SCAN)) {
        consume(state, TOKEN_SCAN, "");
        ASTNode* scan_node = create_node(NODE_SCAN, NULL);
        add_child(scan_node, parse_ArgumentList(state)); // onde vai ser lido.
        consume(state, TOKEN_SEMICOLON, "Esperado ';' após a instrução scan.");
        return scan_node;
    }

    // se for int ou float, é uma declaração de variável
    if (check(state, TOKEN_INT) || check(state, TOKEN_FLOAT)) {
        ASTNode* decl = parse_Declaration(state);
        consume(state, TOKEN_SEMICOLON, "Esperado ';' após a declaração.");
        return decl;
    }
    // se for um identificador seguido de '(' é uma chamada de procedimento
    if (check(state, TOKEN_IDENTIFIER) && state->tokens[state->current + 1].type == TOKEN_LPAREN) {
        ASTNode* call = parse_Factor(state);
        consume(state, TOKEN_SEMICOLON, "Esperado ';' após a chamada de procedimento.");
        return call;
    }
    // se for um identificador sozinho é uma atribuição
    if (check(state, TOKEN_IDENTIFIER)) {
        Token id = consume(state, TOKEN_IDENTIFIER, "");
        // checa se a variável já foi declarada
        if (scope_lookup(state->current_scope, id.lexeme) == NULL) {
            fprintf(stderr, "Erro Semântico: Atribuição a variável não declarada '%s'.\n", id.lexeme); exit(EXIT_FAILURE);
        }
        consume(state, TOKEN_ASSIGN, "Esperado '='.");
        ASTNode* expr = parse_Expression(state);
        consume(state, TOKEN_SEMICOLON, "Esperado ';'.");
        ASTNode* assign = create_node(NODE_ASSIGNMENT, id.lexeme);
        add_child(assign, expr);
        return assign;
    }
    fprintf(stderr, "Erro de Sintaxe na linha %d: Instrução inválida.\n", peek(state).line);
    exit(EXIT_FAILURE);
}

// analisa a lista de parâmetros de uma função ou procedimento
static ASTNode* parse_ParameterList(ParserState* state) {
    ASTNode* params = create_node(NODE_PARAM_LIST, NULL);
    consume(state, TOKEN_LPAREN, "Esperado '('.");
    if (!check(state, TOKEN_RPAREN)) {
        do {
            SymbolDataType type = parse_Type(state);
            Token id = consume(state, TOKEN_IDENTIFIER, "Esperado o nome do parâmetro.");
            // insere o parâmetro na tabela de símbolos do escopo da função
            scope_insert(state->current_scope, id.lexeme, KIND_PARAMETER, type, id.line, state->next_address++);
            add_child(params, create_node(NODE_PARAM, id.lexeme));
        } while (check(state, TOKEN_COMMA) && (consume(state, TOKEN_COMMA, ""), 1));
    }
    consume(state, TOKEN_RPAREN, "Esperado ')'.");
    return params;
}

// analisa a declaração de uma função
static ASTNode* parse_FunctionDeclaration(ParserState* state) {
    consume(state, TOKEN_FUNCTION, "Esperado 'function'.");
    SymbolDataType return_type = parse_Type(state);
    Token id = consume(state, TOKEN_IDENTIFIER, "Esperado o nome da função.");
    // insere a função na tabela de símbolos do escopo global
    scope_insert(state->current_scope, id.lexeme, KIND_FUNCTION, return_type, id.line, 0); 
    ASTNode* func = create_node(NODE_FUNC_DECL, id.lexeme);
    
    // entra em um novo escopo para a função
    state->current_scope = scope_enter(state->current_scope);
    state->next_address = 1;
    
    add_child(func, parse_ParameterList(state));
    add_child(func, parse_ProgramBlock(state));
    
    func->scope = state->current_scope;
    
    // sai do escopo da função, voltando para o pai
    state->current_scope = state->current_scope->parent;
    
    return func;
}

// Analisa um bloco de código
static ASTNode* parse_ProgramBlock(ParserState* state) {
    ASTNode* block = create_node(NODE_BLOCK, NULL);
    consume(state, TOKEN_BEGIN, "Esperado 'begin'.");
    // continua analisando instruções até encontrar um 'end'.
    while (!check(state, TOKEN_END)) {
        add_child(block, parse_Statement(state));
    }
    consume(state, TOKEN_END, "Esperado 'end'.");
    return block;
}

// analisa o nível mais alto do programa
static ASTNode* parse_TopLevel(ParserState* state) {
    ASTNode* program = create_node(NODE_PROGRAM, NULL);
    while (!check(state, TOKEN_EOF)) {
        if (check(state, TOKEN_FUNCTION)) {
            add_child(program, parse_FunctionDeclaration(state)); // se for função, analisa a função
        } else if (check(state, TOKEN_BEGIN)) {
            add_child(program, parse_ProgramBlock(state)); // se for 'begin', eh o bloco principal
            break;
        } else {
            fprintf(stderr, "Erro de Sintaxe: Esperado declaração de função ou bloco principal do programa.\n");
            exit(EXIT_FAILURE);
        }
    }
    return program;
}

// função principal do parser
ASTNode* parse(ParserState* state) {
    ASTNode* root = parse_TopLevel(state);
    if (!check(state, TOKEN_EOF)) {
        fprintf(stderr, "Erro: Tokens extras no final do arquivo, começando com '%s'.\n", peek(state).lexeme);
        exit(EXIT_FAILURE);
    }
    return root;
}
