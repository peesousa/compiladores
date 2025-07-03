#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

//
// tipos de dados que a linguagem de programação vai ter.
//
typedef enum {
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_VOID
} SymbolDataType;

//
// tipo de símbolo
//
typedef enum {
    KIND_VARIABLE,
    KIND_PARAMETER,
    KIND_FUNCTION,
    KIND_PROCEDURE
} SymbolKind;

typedef struct SymbolNode {
    char* name;
    SymbolKind kind;
    SymbolDataType type;
    int line;
    int address;
    struct SymbolNode* next;
} SymbolNode;

#define SYMBOL_TABLE_SIZE 256

typedef struct SymbolTable {
    // tabela de hash.
    SymbolNode* table[SYMBOL_TABLE_SIZE];
    struct SymbolTable* parent;
} SymbolTable;


// cria uma nova tabela de símbolos
SymbolTable* scope_create();

// libera a memória de uma tabela
void scope_destroy(SymbolTable* st);

// entra em um novo escopo
SymbolTable* scope_enter(SymbolTable* parent);

// insere um novo símbolo na tabela
void scope_insert(SymbolTable* st, const char* name, SymbolKind kind, SymbolDataType type, int line, int address);

// orocura por um símbolo na tabela atual e nas tabelas pai
SymbolNode* scope_lookup(SymbolTable* st, const char* name);

// orocura por um símbolo na tabela atual
SymbolNode* scope_lookup_current(SymbolTable* st, const char* name);

// imprime o conteúdo da tabela de símbolos
void scope_dump(SymbolTable* st);

#endif // SYMTAB_H
