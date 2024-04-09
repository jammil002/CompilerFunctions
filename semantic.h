#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "AST.h"
#include "symbolTable.h"
#include "tempVars.h"

int semanticAnalysis(ASTNode *node, SymbolTable *symTab);

#endif // SEMANTIC_H
