// codeGenerator.h

#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "AST.h"
#include "semantic.h"
#include "tac.h"
#include <stdbool.h>

#define NUM_TEMP_REGISTERS 10

void initCodeGenerator(const char *outputFilename, SymbolTable *symTab);
void finalizeCodeGenerator(const char *outputFilename);
void generateMIPS(TAC *tacInstructions);
void deallocateRegister(int regIndex);
int allocateRegister();

#endif // CODE_GENERATOR_H
