#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int hash(const char *key) {
    unsigned long int hash_value = 5381;
    int c;
    while ((c = *key++)) {
        hash_value = ((hash_value << 5) + hash_value) + c;
    }
    return hash_value % SYMBOL_TABLE_SIZE;
}

SymbolTable* scope_create() {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!st) {
        perror("Failed to allocate memory for symbol table");
        exit(EXIT_FAILURE);
    }
    st->parent = NULL;
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        st->table[i] = NULL;
    }
    return st;
}

void scope_destroy(SymbolTable* st) {
    if (!st) return;

    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        SymbolNode* current = st->table[i];
        while (current != NULL) {
            SymbolNode* temp = current;
            current = current->next;
            free(temp->name);
            free(temp);
        }
    }
    free(st);
}

SymbolTable* scope_enter(SymbolTable* parent) {
    SymbolTable* new_scope = scope_create();
    new_scope->parent = parent;
    return new_scope;
}

SymbolNode* scope_lookup_current(SymbolTable* st, const char* name) {
    if (!st) return NULL;
    unsigned int index = hash(name);
    SymbolNode* current = st->table[index];
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void scope_insert(SymbolTable* st, const char* name, SymbolKind kind, SymbolDataType type, int line, int address) {
    if (scope_lookup_current(st, name) != NULL) {
        fprintf(stderr, "Semantic Error on line %d: Identifier '%s' already declared in this scope.\n", line, name);
        exit(EXIT_FAILURE);
    }

    unsigned int index = hash(name);
    SymbolNode* new_node = (SymbolNode*)malloc(sizeof(SymbolNode));
    if (!new_node) {
        perror("Failed to allocate memory for symbol node");
        exit(EXIT_FAILURE);
    }

    new_node->name = strdup(name);
    new_node->kind = kind;
    new_node->type = type;
    new_node->line = line;
    new_node->address = address;
    
    new_node->next = st->table[index];
    st->table[index] = new_node;
}

SymbolNode* scope_lookup(SymbolTable* st, const char* name) {
    SymbolTable* current_scope = st;
    while (current_scope != NULL) {
        SymbolNode* symbol = scope_lookup_current(current_scope, name);
        if (symbol != NULL) {
            return symbol;
        }
        current_scope = current_scope->parent;
    }
    return NULL;
}

void scope_dump(SymbolTable* st) {
    printf("--- Symbol Table Dump ---\n");
    int scope_level = 0;
    SymbolTable* current_scope = st;
    while(current_scope) {
        printf("--- Scope Level %d ---\n", scope_level++);
        for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
            SymbolNode* current = current_scope->table[i];
            if (current) {
                while (current) {
                    printf("  -> Name: %-15s, Kind: %d, Type: %d, Addr: %d\n",
                           current->name, current->kind, current->type, current->address);
                    current = current->next;
                }
            }
        }
        current_scope = current_scope->parent;
    }
    printf("-------------------------\n");
}
