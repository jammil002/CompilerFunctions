#ifndef SYMBOL_TABLE1_H
#define SYMBOL_TABLE1_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Symbol structure

typedef struct Symbol
{
    char *name;
    char *type;
    int scopeLevel;
    struct Symbol *next;

    bool isFunction;
    struct ASTNode *parameters;

    bool isArray;
    int arraySize;
} Symbol;

// Define the SymbolTable struct
typedef struct SymbolTable
{
    int size;
    struct Symbol **table;
} SymbolTable;

// Globals
#define MAX_SCOPE_LEVEL 100
static SymbolTable *scopeStack[MAX_SCOPE_LEVEL];
static int currentScopeLevel = -1;

// Function declarations
void addSymbol(SymbolTable *table, char *name, char *type);
Symbol *lookupSymbol(SymbolTable *table, char *name);
void printSymbolTable(SymbolTable *table);
SymbolTable *createSymbolTable(int size);
void freeSymbolTable(SymbolTable *table);
void enterScope();
void exitScope();

#endif // SYMBOL_TABL1_H
