#include "tac.h"

TAC *tacHead = NULL;
int tempVars[20] = {0};

TAC *generateTACForExpr(ASTNode *expr)
{
    if (!expr)
        return NULL;

    TAC *instruction = (TAC *)malloc(sizeof(TAC));
    if (!instruction)
    {
        fprintf(stderr, "generateTACForExpr: Memory allocation failed for TAC instruction\n");
        return NULL;
    }

    // Initialize fields to ensure clean state
    instruction->arg1 = instruction->arg2 = instruction->op = instruction->result = NULL;
    instruction->next = NULL;

    switch (expr->type)
    {
    case NodeType_Expr:
        printf("generateTACForExpr: Generating TAC for Expression\n");
        instruction->arg1 = createOperand(expr->expr.left);
        instruction->arg2 = createOperand(expr->expr.right);
        instruction->op = strdup(expr->expr.operator);
        instruction->result = createTempVar();
        break;

    case NodeType_SimpleExpr:
        printf("generateTACForExpr: Generating TAC for Simple Expression\n");
        char buffer[20]; // Buffer for number to string conversion
        snprintf(buffer, sizeof(buffer), "%d", expr->simpleExpr.number);
        instruction->arg1 = strdup(buffer);
        instruction->op = strdup("li");
        instruction->result = createTempVar();
        break;

    case NodeType_SimpleID:
        printf("generateTACForExpr: Generating TAC for Simple ID\n");
        // For a simple ID, we typically do not generate a TAC unless it's being used in an operation.
        // However, in the context of your implementation, this could be different.
        free(instruction); // Avoid memory leak since we're not using this structure.
        return NULL;

    case NodeType_AssignStmt:
        printf("generateTACForExpr: Generating TAC for Assignment Statement\n");
        instruction->arg1 = createOperand(expr->assignStmt.expr); // Right-hand side of assignment
        instruction->op = strdup("=");
        instruction->result = strdup(expr->assignStmt.varName);
        break;

    case NodeType_WriteStmt:
        printf("generateTACForExpr: Generating TAC for Write Statement\n");
        instruction->arg1 = createOperand(expr->writeStmt.expr); // Expression to write
        instruction->op = strdup("write");
        instruction->result = NULL; // No result needed for write operation
        break;

    case NodeType_BinOp:
        printf("generateTACForExpr: Generating TAC for Binary Operation\n");
        instruction->arg1 = createOperand(expr->binOp.left);
        instruction->arg2 = createOperand(expr->binOp.right);
        instruction->op = strdup(expr->binOp.operator);
        instruction->result = createTempVar();
        break;

    case NodeType_FunctionCall:
        printf("generateTACForExpr: Generating TAC for Function Call\n");
        instruction->arg1 = strdup(expr->funcCall.funcName);
        instruction->op = strdup("call");
        instruction->result = createTempVar(); // TODO Functions might return a value.
        break;

    case NodeType_ArrayAccess:
        printf("generateTACForExpr: Generating TAC for Array Access\n");
        instruction->arg1 = strdup(expr->arrayAccess.arrayName);
        instruction->arg2 = createOperand(expr->arrayAccess.indexExpr);
        instruction->op = strdup("array_load");
        instruction->result = createTempVar();
        break;

        // TODO Add more cases as needed for your specific AST and TAC requirements.

    default:
        printf("generateTACForExpr: Unhandled node type in TAC generation: %d\n", expr->type);
        free(instruction); // Avoid memory leak
        return NULL;
    }

    // Print the TAC for debugging before appending
    if (instruction->op != NULL)
    { // Make sure it's a valid TAC before printing
        printf("Generated TAC: ");
        printTAC(instruction); // Function to print single TAC line
    }

    // Append this instruction to your list of TAC instructions if it's a valid instruction
    if (instruction->result != NULL || instruction->op != NULL)
    {
        appendTAC(&tacHead, instruction);
    }
    else
    {
        // If it turned out to be an instruction we don't need to append, clean up.
        free(instruction);
    }

    return instruction;
}

char *createTempVar()
{
    static int count = 0;
    char *tempVar = malloc(10); // Enough space for "t" + number
    if (!tempVar)
        return NULL;
    count = allocateNextAvailableTempVar(tempVars);
    sprintf(tempVar, "t%d", count++);
    return tempVar;
}

char *createOperand(ASTNode *node)
{
    if (!node)
        return strdup(""); // Safety check

    char buffer[64]; // Buffer for creating string representations

    switch (node->type)
    {
    case NodeType_SimpleExpr: // Handle simple numeric expressions
        snprintf(buffer, sizeof(buffer), "%d", node->simpleExpr.number);
        return strdup(buffer);
    case NodeType_SimpleID: // Handle identifiers
        return strdup(node->simpleID.name);
    case NodeType_ArrayAccess:
    { // Note the opening brace to introduce a new scope
        char *indexStr = createOperand(node->arrayAccess.indexExpr);
        snprintf(buffer, sizeof(buffer), "%s[%s]", node->arrayAccess.arrayName, indexStr);
        free(indexStr); // Cleanup the operand string used for the index
        return strdup(buffer);
    } // Close the scope for this case
    default:
        fprintf(stderr, "createOperand: Unknown or unsupported node type %d\n", node->type);
        return strdup("unknown");
    }
}

void printTAC(TAC *tac)
{
    if (!tac)
        return;

    // Check if the operation is 'write', which might not have a result
    if (tac->op && strcmp(tac->op, "write") == 0)
    {
        printf("write %s\n", tac->arg1 ? tac->arg1 : "(null)");
    }
    else
    {
        // Handle general case with checks for NULL values
        printf("%s = %s %s %s\n",
               tac->result ? tac->result : "(null)",
               tac->arg1 ? tac->arg1 : "(null)",
               tac->op ? tac->op : "(null)",
               tac->arg2 ? tac->arg2 : "(null)");
    }
}

void printTACToFile(const char *filename, TAC *tac)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Failed to open file");
        return;
    }

    TAC *current = tac;
    while (current != NULL)
    {
        // Handle 'write' operation specifically
        if (current->op && strcmp(current->op, "write") == 0)
        {
            fprintf(file, "write %s\n", current->arg1 ? current->arg1 : "(null)");
        }
        else
        {
            // Handle general case with checks for NULL values
            fprintf(file, "%s = %s %s %s\n",
                    current->result ? current->result : "(null)",
                    current->arg1 ? current->arg1 : "(null)",
                    current->op ? current->op : "(null)",
                    current->arg2 ? current->arg2 : "(null)");
        }
        current = current->next;
    }
    fclose(file);
    printf("TAC written to %s\n", filename);
}

// Temporary variable allocation and deallocation functions //

void initializeTempVars()
{
    for (int i = 0; i < 20; i++)
    {
        tempVars[i] = 0;
    }
}

int allocateNextAvailableTempVar(int tempVars[])
{
    // implement the temp var allocation logic
    // use the tempVars array to keep track of allocated temp vars

    // search for the next available temp var
    for (int i = 0; i < 20; i++)
    {
        if (tempVars[i] == 0)
        {
            tempVars[i] = 1;
            return i;
        }
    }
    return -1; // No available temp var
}

void deallocateTempVar(int tempVars[], int index)
{
    // implement the temp var deallocation logic
    // use the tempVars array to keep track of allocated temp vars
    if (index >= 0 && index < 20)
    {
        tempVars[index] = 0;
    }
}

void appendTAC(TAC **head, TAC *newInstruction)
{
    if (!*head)
    {
        *head = newInstruction;
    }
    else
    {
        TAC *current = *head;
        while (current->next)
        {
            current = current->next;
        }
        current->next = newInstruction;
    }
}