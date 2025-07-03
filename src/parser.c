#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

static ASTNode* parse_Statement(ParserState* state);
static ASTNode* parse_Expression(ParserState* state);
static ASTNode* parse_ProgramBlock(ParserState* state);
static ASTNode* parse_FunctionDeclaration(ParserState* state);
static ASTNode* parse_Conditional(ParserState* state);
static ASTNode* parse_Loop(ParserState* state);
static ASTNode* parse_Factor(ParserState* state);
static ASTNode* parse_Term(ParserState* state);
static SymbolDataType parse_Type(ParserState* state);

static Token peek(ParserState* state) { return state->tokens[state->current]; }
static void advance(ParserState* state) { if (state->current < state->token_count) state->current++; }
static Token consume(ParserState* state, TokenType type, const char* message) {
    if (peek(state).type == type) {
        Token t = peek(state); advance(state); return t;
    }
    fprintf(stderr, "Syntax Error on line %d: %s. Found '%s' (%s) instead.\n", peek(state).line, message, peek(state).lexeme, token_type_to_string(peek(state).type));
    exit(EXIT_FAILURE);
}
static int check(ParserState* state, TokenType type) { return peek(state).type == type; }

static SymbolDataType parse_Type(ParserState* state) {
    if (check(state, TOKEN_INT)) { consume(state, TOKEN_INT, ""); return TYPE_INTEGER; }
    if (check(state, TOKEN_FLOAT)) { consume(state, TOKEN_FLOAT, ""); return TYPE_FLOAT; }
    fprintf(stderr, "Syntax Error: Expected a type.\n"); exit(EXIT_FAILURE);
}

static ASTNode* parse_ArgumentList(ParserState* state) {
    ASTNode* args = create_node(NODE_ARG_LIST, NULL);
    consume(state, TOKEN_LPAREN, "Expected '('.");
    if (!check(state, TOKEN_RPAREN)) {
        do { add_child(args, parse_Expression(state)); } while (check(state, TOKEN_COMMA) && (consume(state, TOKEN_COMMA, ""), 1));
    }
    consume(state, TOKEN_RPAREN, "Expected ')'.");
    return args;
}

static ASTNode* parse_Factor(ParserState* state) {
    if (check(state, TOKEN_IDENTIFIER) && state->tokens[state->current + 1].type == TOKEN_LPAREN) {
        Token id = consume(state, TOKEN_IDENTIFIER, "");
        SymbolNode* sym = scope_lookup(state->current_scope, id.lexeme);
        if (!sym || (sym->kind != KIND_FUNCTION && sym->kind != KIND_PROCEDURE)) {
            fprintf(stderr, "Semantic Error: '%s' is not a function.\n", id.lexeme); exit(EXIT_FAILURE);
        }
        ASTNode* call = create_node(NODE_FUNC_CALL, id.lexeme);
        add_child(call, parse_ArgumentList(state));
        return call;
    }
    if (check(state, TOKEN_IDENTIFIER)) {
        Token id = consume(state, TOKEN_IDENTIFIER, "");
        if (scope_lookup(state->current_scope, id.lexeme) == NULL) {
            fprintf(stderr, "Semantic Error: Variable '%s' not declared.\n", id.lexeme); exit(EXIT_FAILURE);
        }
        return create_node(NODE_IDENTIFIER, id.lexeme);
    }
    if (check(state, TOKEN_INTEGER_LITERAL)) return create_node(NODE_INT_LITERAL, consume(state, TOKEN_INTEGER_LITERAL, "").lexeme);
    if (check(state, TOKEN_LPAREN)) {
        consume(state, TOKEN_LPAREN, "");
        ASTNode* expr = parse_Expression(state);
        consume(state, TOKEN_RPAREN, "");
        return expr;
    }
    fprintf(stderr, "Syntax Error: Unexpected token in expression.\n"); exit(EXIT_FAILURE);
}

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
            default: type = -1; // Should not happen
        }
        ASTNode* new_node = create_node(type, NULL);
        add_child(new_node, node); add_child(new_node, right);
        node = new_node;
    }
    return node;
}

static ASTNode* parse_Declaration(ParserState* state) {
    SymbolDataType type = parse_Type(state);
    Token id = consume(state, TOKEN_IDENTIFIER, "Expected identifier.");
    scope_insert(state->current_scope, id.lexeme, KIND_VARIABLE, type, id.line, state->next_address++);
    return create_node(NODE_DECLARATION, id.lexeme);
}

static ASTNode* parse_ReturnStatement(ParserState* state) {
    consume(state, TOKEN_RETURN, "Expected 'return'.");
    ASTNode* ret = create_node(NODE_RETURN_STMT, NULL);
    add_child(ret, parse_Expression(state));
    consume(state, TOKEN_SEMICOLON, "Expected ';'.");
    return ret;
}


    

static ASTNode* parse_Conditional(ParserState* state) {
    consume(state, TOKEN_IF, "Expected 'if'.");
    ASTNode* cond_node = create_node(NODE_CONDITIONAL, NULL);
    add_child(cond_node, parse_Expression(state));
    consume(state, TOKEN_THEN, "Expected 'then'.");
    add_child(cond_node, parse_ProgramBlock(state));
    if (check(state, TOKEN_ELSE)) {
        consume(state, TOKEN_ELSE, "");
        add_child(cond_node, parse_ProgramBlock(state));
        consume(state, TOKEN_ENDELSE, "Expected 'endelse'.");
    }
    consume(state, TOKEN_ENDIF, "Expected 'endif'.");
    return cond_node;
}

static ASTNode* parse_Loop(ParserState* state) {
    consume(state, TOKEN_WHILE, "Expected 'while'.");
    ASTNode* loop_node = create_node(NODE_LOOP, NULL);
    add_child(loop_node, parse_Expression(state));
    consume(state, TOKEN_DO, "Expected 'do'.");
    add_child(loop_node, parse_ProgramBlock(state));
    consume(state, TOKEN_ENDWHILE, "Expected 'endwhile'.");
    return loop_node;
}

static ASTNode* parse_Statement(ParserState* state) {
    if (check(state, TOKEN_IF)) return parse_Conditional(state);
    if (check(state, TOKEN_WHILE)) return parse_Loop(state);
    if (check(state, TOKEN_RETURN)) return parse_ReturnStatement(state);
    if (check(state, TOKEN_INT) || check(state, TOKEN_FLOAT)) {
        ASTNode* decl = parse_Declaration(state);
        consume(state, TOKEN_SEMICOLON, "Expected ';' after declaration.");
        return decl;
    }
    if (check(state, TOKEN_IDENTIFIER) && state->tokens[state->current + 1].type == TOKEN_LPAREN) {
        ASTNode* call = parse_Factor(state);
        consume(state, TOKEN_SEMICOLON, "Expected ';' after procedure call.");
        return call;
    }
    if (check(state, TOKEN_IDENTIFIER)) {
        Token id = consume(state, TOKEN_IDENTIFIER, "");
        if (scope_lookup(state->current_scope, id.lexeme) == NULL) {
            fprintf(stderr, "Semantic Error: Assignment to undeclared variable '%s'.\n", id.lexeme); exit(EXIT_FAILURE);
        }
        consume(state, TOKEN_ASSIGN, "Expected '='.");
        ASTNode* expr = parse_Expression(state);
        consume(state, TOKEN_SEMICOLON, "Expected ';'.");
        ASTNode* assign = create_node(NODE_ASSIGNMENT, id.lexeme);
        add_child(assign, expr);
        return assign;
    }
    fprintf(stderr, "Syntax Error on line %d: Invalid statement.\n", peek(state).line);
    exit(EXIT_FAILURE);
}

static ASTNode* parse_ParameterList(ParserState* state) {
    ASTNode* params = create_node(NODE_PARAM_LIST, NULL);
    consume(state, TOKEN_LPAREN, "Expected '('.");
    if (!check(state, TOKEN_RPAREN)) {
        do {
            SymbolDataType type = parse_Type(state);
            Token id = consume(state, TOKEN_IDENTIFIER, "Expected param name.");
            scope_insert(state->current_scope, id.lexeme, KIND_PARAMETER, type, id.line, state->next_address++);
            add_child(params, create_node(NODE_PARAM, id.lexeme));
        } while (check(state, TOKEN_COMMA) && (consume(state, TOKEN_COMMA, ""), 1));
    }
    consume(state, TOKEN_RPAREN, "Expected ')'.");
    return params;
}

static ASTNode* parse_FunctionDeclaration(ParserState* state) {
    consume(state, TOKEN_FUNCTION, "Expected 'function'.");
    SymbolDataType return_type = parse_Type(state);
    Token id = consume(state, TOKEN_IDENTIFIER, "Expected function name.");
    scope_insert(state->current_scope, id.lexeme, KIND_FUNCTION, return_type, id.line, 0); 
    ASTNode* func = create_node(NODE_FUNC_DECL, id.lexeme);
    state->current_scope = scope_enter(state->current_scope);
    state->next_address = 1;
    add_child(func, parse_ParameterList(state));
    add_child(func, parse_ProgramBlock(state));
    
    func->scope = state->current_scope;
    
    state->current_scope = state->current_scope->parent;
    
    return func;
}

static ASTNode* parse_ProgramBlock(ParserState* state) {
    ASTNode* block = create_node(NODE_BLOCK, NULL);
    consume(state, TOKEN_BEGIN, "Expected 'begin'.");
    while (!check(state, TOKEN_END)) {
        add_child(block, parse_Statement(state));
    }
    consume(state, TOKEN_END, "Expected 'end'.");
    return block;
}

static ASTNode* parse_TopLevel(ParserState* state) {
    ASTNode* program = create_node(NODE_PROGRAM, NULL);
    while (!check(state, TOKEN_EOF)) {
        if (check(state, TOKEN_FUNCTION)) {
            add_child(program, parse_FunctionDeclaration(state));
        } else if (check(state, TOKEN_BEGIN)) {
            add_child(program, parse_ProgramBlock(state));
            break;
        } else {
            fprintf(stderr, "Syntax Error: Expected function declaration or main program block.\n");
            exit(EXIT_FAILURE);
        }
    }
    return program;
}

ASTNode* parse(ParserState* state) {
    ASTNode* root = parse_TopLevel(state);
    if (!check(state, TOKEN_EOF)) {
        fprintf(stderr, "Error: Extra tokens at end of file, starting with '%s'.\n", peek(state).lexeme);
        exit(EXIT_FAILURE);
    }
    return root;
}
