#include "../symbolTable.h"
#include <stdio.h>

int main()
{
    SymbolTable *table = createSymbolTable(100); // Adjust size as needed

    addSymbol(table, "x", "int");
    addSymbol(table, "y", "float");

    printSymbolTable(table);

    Symbol *x = findSymbol(table, "x");
    if (x != NULL)
    {
        printf("Found symbol: %s of type %s\n", x->name, x->type);
    }
    else
    {
        printf("Symbol x not found\n");
    }

    freeSymbolTable(table);
    return 0;
}
