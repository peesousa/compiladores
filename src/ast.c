#include "ast.h"
#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ASTNode* create_node(NodeType type, const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        perror("Failed to allocate ASTNode");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->child = NULL;
    node->sibling = NULL;
    node->scope = NULL;
    return node;
}

void add_child(ASTNode* parent, ASTNode* new_child) {
    if (!parent || !new_child) return;
    if (parent->child == NULL) {
        parent->child = new_child;
    } else {
        ASTNode* current_sibling = parent->child;
        while (current_sibling->sibling != NULL) {
            current_sibling = current_sibling->sibling;
        }
        current_sibling->sibling = new_child;
    }
}

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

void print_ast(ASTNode* node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("Type: %d", node->type);
    if (node->value) printf(", Value: \"%s\"", node->value);
    printf("\n");
    print_ast(node->child, level + 1);
    print_ast(node->sibling, level);
}
