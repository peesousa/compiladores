#ifndef SYMTAB_H
#define SYMTAB_H

#include "ast.h"

typedef enum {
    TYPE_INTEGER,
    TYPE_FLOAT,
    TYPE_CHAR,
    TYPE_VOID
} SymbolDataType;

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
    SymbolNode* table[SYMBOL_TABLE_SIZE];
    struct SymbolTable* parent;
} SymbolTable;


SymbolTable* scope_create();

void scope_destroy(SymbolTable* st);

SymbolTable* scope_enter(SymbolTable* parent);

void scope_insert(SymbolTable* st, const char* name, SymbolKind kind, SymbolDataType type, int line, int address);

SymbolNode* scope_lookup(SymbolTable* st, const char* name);

SymbolNode* scope_lookup_current(SymbolTable* st, const char* name);

void scope_dump(SymbolTable* st);

#endif // SYMTAB_H
