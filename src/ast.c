#include "ast.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cria um novo nó para a AST
ASTNode* create_node(NodeType type, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Falha ao alocar ASTNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->child = NULL;
    node->sibling = NULL;
    node->scope = NULL;
    return node;
}

// adiciona um nó filho a um nó pai
void add_child(ASTNode* parent, ASTNode* new_child) {
    if (!parent || !new_child) return;
    if (parent->child == NULL) {
        parent->child = new_child;
    } else {
        // se já tem filho, vai até o último irmão e adiciona o novo lá
        ASTNode* current_sibling = parent->child;
        while (current_sibling->sibling != NULL) {
            current_sibling = current_sibling->sibling;
        }
        current_sibling->sibling = new_child;
    }
}

// libera toda a memória que a AST usou
void free_ast(ASTNode* root) {
    if (!root) return;
    free_ast(root->sibling);
    free_ast(root->child);
    if (root->scope) {
        scope_destroy(root->scope);
    }
    free(root->value);
    free(root);
}

// imprime a AST
void print_ast(ASTNode* node, int level) {
    if (!node)
      return;

    for (int i = 0; i < level; i++)
      printf("  ");

    printf("Tipo: %d", node->type); // Imprime o tipo do nó
    if (node->value)
      printf(", Valor: \"%s\"", node->value); // imprime se tiver valor

    printf("\n");
    print_ast(node->child, level + 1);
    print_ast(node->sibling, level);
}
