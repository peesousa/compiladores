#include "symtab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// função de "hash"zer a busca por variáveis ser bem rápida.
static unsigned int hash(const char *key) {
    unsigned long int hash_value = 5381;
    int c;
    while ((c = *key++)) {
        hash_value = ((hash_value << 5) + hash_value) + c; // djb2 hash
    }
    return hash_value % SYMBOL_TABLE_SIZE;
}

// cria uma nova tabela de símbolos
SymbolTable* scope_create() {
    SymbolTable* st = (SymbolTable*)malloc(sizeof(SymbolTable));
    if (!st) {
        perror("Falha ao alocar memória para a tabela de símbolos");
        exit(EXIT_FAILURE);
    }
    st->parent = NULL;

    // inicializa todas as posições da tabela como vazias
    for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
        st->table[i] = NULL;
    }
    return st;
}

// libera a memória que a gente usou para a tabela de símbolos
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

// cria um novo escopo dentro de outro
SymbolTable* scope_enter(SymbolTable* parent) {
    SymbolTable* new_scope = scope_create();
    new_scope->parent = parent; // o escopo atual se torna o pai do novo
    return new_scope;
}

// procura por um símbolo (variável ou função) no escopo atual.
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

// insere uma nova variável ou função na tabela de símbolos
void scope_insert(SymbolTable* st, const char* name, SymbolKind kind, SymbolDataType type, int line, int address) {
    // checa se já existe uma variável com esse nome
    if (scope_lookup_current(st, name) != NULL) {
        fprintf(stderr, "Erro Semântico na linha %d: Identificador '%s' já foi declarado neste escopo.\n", line, name);
        exit(EXIT_FAILURE);
    }

    unsigned int index = hash(name);
    SymbolNode* new_node = (SymbolNode*)malloc(sizeof(SymbolNode));
    if (!new_node) {
        perror("Falha ao alocar memória para o nó do símbolo");
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

// procura por um símbolo começando pelo escopo atual e vai subindo para os escopos pai se não achar
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

// despejar o conteúdo da tabela de símbolos na tela
void scope_dump(SymbolTable* st) {
    printf("--- Despejo da Tabela de Símbolos ---\n");
    int scope_level = 0;
    SymbolTable* current_scope = st;
    while(current_scope) {
        printf("--- Nível de Escopo %d ---\n", scope_level++);
        for (int i = 0; i < SYMBOL_TABLE_SIZE; i++) {
            SymbolNode* current = current_scope->table[i];
            if (current) {
                while (current) {
                    printf("  -> Nome: %-15s, Tipo: %d, Categoria: %d, Endereço: %d\n",
                           current->name, current->type, current->kind, current->address);
                    current = current->next;
                }
            }
        }
        current_scope = current_scope->parent;
    }
    printf("------------------------------------\n");
}
