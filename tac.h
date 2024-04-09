#ifndef TAC_H
#define TAC_H

#include "AST.h"
#include "symbolTable.h"

typedef struct TAC
{
    char *op;
    char *arg1;
    char *arg2;
    char *result;
    struct TAC *next;
} TAC;

extern TAC *tacHead;

void printTACToFile(const char *filename, TAC *tac);
void deallocateTempVar(int tempVars[], int index);
int allocateNextAvailableTempVar(int tempVars[]);
int allocateNextAvailableTempVar(int tempVars[]);
void appendTAC(TAC **head, TAC *newInstruction);
TAC *generateTACForExpr(struct ASTNode *expr);
char *createOperand(struct ASTNode *node);
void initializeTempVars();
void printTAC(TAC *tac);
char *createTempVar();

#endif // TAC_H