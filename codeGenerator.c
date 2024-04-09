
#include "codeGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

static FILE *outputFile;

typedef struct
{
    char *name;
    bool inUse;
} MIPSRegister;

MIPSRegister tempRegisters[NUM_TEMP_REGISTERS] = {
    {"$t0", false}, {"$t1", false}, {"$t2", false}, {"$t3", false}, {"$t4", false}, {"$t5", false}, {"$t6", false}, {"$t7", false}, {"$t8", false}, {"$t9", false}};

void initCodeGenerator(const char *outputFilename, SymbolTable *symTab)
{
    outputFile = fopen(outputFilename, "w");
    if (outputFile == NULL)
    {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
    fprintf(outputFile, ".data\n");

    for (int i = 0; i < symTab->size; i++)
    {
        for (Symbol *sym = symTab->table[i]; sym != NULL; sym = sym->next)
        {
            fprintf(outputFile, "%s: .word 0\n", sym->name); // Allocate space for each variable
        }
    }
    fprintf(outputFile, "newline: .asciiz \"\\n\"\n"); // For newline in write operations
}

void generateMIPS(TAC *tacInstructions)
{
    TAC *current = tacInstructions;

    fprintf(outputFile, ".text\n.globl main\nmain:\n");

    while (current != NULL)
    {
        if (strcmp(current->op, "assign") == 0)
        {
            int resReg = allocateRegister(); // Register for the result / right-hand side value

            if (resReg == -1)
            {
                fprintf(stderr, "Run out of registers for assignment\n");
                exit(EXIT_FAILURE); // Real compiler should handle more gracefully
            }

            if (isdigit(current->arg1[0]) || (current->arg1[0] == '-' && isdigit(current->arg1[1])))
            {
                fprintf(outputFile, "\tli %s, %s\n", tempRegisters[resReg].name, current->arg1); // Load immediate value
            }
            else
            {
                fprintf(outputFile, "\tlw %s, %s\n", tempRegisters[resReg].name, current->arg1); // Load value from variable
            }
            fprintf(outputFile, "\tsw %s, %s\n", tempRegisters[resReg].name, current->result); // Store it in the result variable

            deallocateRegister(resReg); // Free up the register after use
        }
        else if (strcmp(current->op, "+") == 0)
        {
            int reg1 = allocateRegister();
            int reg2 = allocateRegister();
            int resReg = allocateRegister(); // Result register

            if (reg1 == -1 || reg2 == -1 || resReg == -1)
            {
                fprintf(stderr, "Run out of registers for addition\n");
                exit(EXIT_FAILURE); // TODO Fix this
            }

            fprintf(outputFile, "\tlw %s, %s\n", tempRegisters[reg1].name, current->arg1);                                             // Load first operand
            fprintf(outputFile, "\tlw %s, %s\n", tempRegisters[reg2].name, current->arg2);                                             // Load second operand
            fprintf(outputFile, "\tadd %s, %s, %s\n", tempRegisters[resReg].name, tempRegisters[reg1].name, tempRegisters[reg2].name); // Add them
            fprintf(outputFile, "\tsw %s, %s\n", tempRegisters[resReg].name, current->result);                                         // Store result

            deallocateRegister(reg1); // Freeing up the registers after use
            deallocateRegister(reg2);
            deallocateRegister(resReg);
        }
        else if (strcmp(current->op, "write") == 0)
        {
            int argReg = allocateRegister(); // Register for the argument

            if (argReg == -1)
            {
                fprintf(stderr, "Run out of registers for write operation\n");
                exit(EXIT_FAILURE);
            }

            fprintf(outputFile, "\tlw %s, %s\n", tempRegisters[argReg].name, current->arg1); // Load the variable's value into register
            fprintf(outputFile, "\tmove $a0, %s\n", tempRegisters[argReg].name);             // Move the value to $a0 for printing
            fprintf(outputFile, "\tli $v0, 1\n");                                            // Set $v0 to 1 for print_int syscall
            fprintf(outputFile, "\tsyscall\n");                                              // Make the syscall
            fprintf(outputFile, "\tli $v0, 4\n");                                            // Set $v0 to 4 for print_string syscall
            fprintf(outputFile, "\tla $a0, newline\n");                                      // Load address of newline character
            fprintf(outputFile, "\tsyscall\n");                                              // Print newline

            deallocateRegister(argReg); // Free up the register after use
        }
        // TODO Add subtraction, multiplication, division, handle arrays.

        current = current->next;
    }
    fprintf(outputFile, "\tli $v0, 10\n"); // Exit syscall
    fprintf(outputFile, "\tsyscall\n");
}

void finalizeCodeGenerator(const char *outputFilename)
{
    if (outputFile)
    {
        fclose(outputFile);
        printf("MIPS code generated and saved to file %s\n", outputFilename);
        outputFile = NULL;
    }
}

int allocateRegister()
{
    for (int i = 0; i < NUM_TEMP_REGISTERS; i++)
    {
        if (!tempRegisters[i].inUse)
        {
            tempRegisters[i].inUse = true;
            return i;
        }
    }
    return -1;
}

void deallocateRegister(int regIndex)
{
    if (regIndex >= 0 && regIndex < NUM_TEMP_REGISTERS)
    {
        tempRegisters[regIndex].inUse = false;
    }
}
