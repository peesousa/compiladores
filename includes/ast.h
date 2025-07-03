#ifndef AST_H
#define AST_H

struct SymbolTable;

typedef enum {
    // estruturais
    NODE_PROGRAM, NODE_BLOCK, NODE_DECLARATION,
    // statements
    NODE_ASSIGNMENT, NODE_LOOP, NODE_CONDITIONAL, NODE_PRINT, NODE_SCAN, NODE_RETURN_STMT,
    // funções
    NODE_FUNC_DECL, NODE_PROC_DECL, NODE_PARAM_LIST, NODE_PARAM, NODE_FUNC_CALL, NODE_ARG_LIST,
    // expressões e operadores
    NODE_ADD, NODE_SUB, NODE_MUL, NODE_DIV, NODE_NEGATE, NODE_EQ, NODE_NEQ,
    NODE_LT, NODE_LTE, NODE_GT, NODE_GTE,
    // literais e tipos
    NODE_IDENTIFIER, NODE_INT_LITERAL, NODE_FLOAT_LITERAL, NODE_TYPE_INT, NODE_TYPE_FLOAT
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char* value;
    struct ASTNode* child;
    struct ASTNode* sibling;
    struct SymbolTable* scope; 
} ASTNode;

ASTNode* create_node(NodeType type, const char* value);
void add_child(ASTNode* parent, ASTNode* new_child);
void free_ast(ASTNode* root);
void print_ast(ASTNode* node, int level);

#endif // AST_H
