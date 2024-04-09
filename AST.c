#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AST.h"

void ASTtoTAC(ASTNode *node)
{
    if (!node)
    {
        printf("ASTtoTAC: Null node encountered.\n");
        return;
    }

    printf("ASTtoTAC: Processing node type %d.\n", node->type);

    switch (node->type)
    {
    case NodeType_Program:
        printf("ASTtoTAC: NodeType_Program\n");
        ASTtoTAC(node->program.varDeclList);
        ASTtoTAC(node->program.stmtList);
        break;

    case NodeType_VarDeclList:
        printf("ASTtoTAC: NodeType_VarDeclList\n");
        ASTtoTAC(node->varDeclList.varDecl);
        ASTtoTAC(node->varDeclList.varDeclList);
        break;

    case NodeType_StmtList:
        printf("ASTtoTAC: NodeType_StmtList\n");
        ASTtoTAC(node->stmtList.stmt);
        ASTtoTAC(node->stmtList.stmtList);
        break;

    case NodeType_Expr:
    case NodeType_SimpleExpr:
    case NodeType_SimpleID:
    case NodeType_BinOp:
    case NodeType_AssignStmt:
    case NodeType_FunctionCall:
    case NodeType_ArrayAccess:
    case NodeType_WriteStmt:
        printf("ASTtoTAC: NodeType involving expression or statement\n");
        generateTACForExpr(node);
        break;

    case NodeType_VarDecl:
        printf("ASTtoTAC: NodeType_VarDecl\n");
        // TODO VarDecl might not directly translate to TAC but may be involved in symbol table management
        break;

    case NodeType_FunctionDecl:
        printf("ASTtoTAC: NodeType_FunctionDecl\n");
        ASTtoTAC(node->funcDecl.paramList);
        ASTtoTAC(node->funcDecl.funcBody);
        break;

    case NodeType_ParamList:
        printf("ASTtoTAC: NodeType_ParamList\n");
        ASTtoTAC(node->paramList.param);
        ASTtoTAC(node->paramList.paramList);
        break;

    case NodeType_Param:
        printf("ASTtoTAC: NodeType_Param\n");
        // TODO Parameters might not directly translate to TAC but are important for function calls
        break;

    case NodeType_ArgList:
        printf("ASTtoTAC: NodeType_ArgList\n");
        ASTtoTAC(node->argList.arg);
        ASTtoTAC(node->argList.argList);
        break;

    case NodeType_Arg:
        printf("ASTtoTAC: NodeType_Arg\n");
        ASTtoTAC(node->arg.arg);
        break;

    case NodeType_ArrayDecl:
        printf("ASTtoTAC: NodeType_ArrayDecl\n");
        // TODO Array declaration might influence symbol table but does not directly result in TAC
        break;

    default:
        printf("ASTtoTAC: Unhandled node type: %d\n", node->type);
        break;
    }
}

void traverseAST(ASTNode *node, int level)
{
    if (!node)
    {
        printf("Nothing to traverse\n");
        return;
    }

    printBranches(level);

    switch (node->type)
    {
    case NodeType_Program:
        printf("Program (line %d)\n", node->lineno);
        traverseAST(node->program.varDeclList, level + 1);
        traverseAST(node->program.stmtList, level + 1);
        break;
    case NodeType_VarDeclList:
        traverseAST(node->varDeclList.varDecl, level + 1);
        traverseAST(node->varDeclList.varDeclList, level + 1);
        break;
    case NodeType_VarDecl:
        printf("VarDecl: %s %s (line %d)\n", node->varDecl.varType, node->varDecl.varName, node->lineno);
        break;
    case NodeType_SimpleExpr:
        printf("%d (line %d)\n", node->simpleExpr.number, node->lineno);
        break;
    case NodeType_SimpleID:
        printf("%s (line %d)\n", node->simpleID.name, node->lineno);
        break;
    case NodeType_Expr:
        printf("Expr: %s (line %d)\n", node->expr.operator, node->lineno);
        traverseAST(node->expr.left, level + 1);
        traverseAST(node->expr.right, level + 1);
        break;
    case NodeType_StmtList:
        traverseAST(node->stmtList.stmt, level + 1);
        traverseAST(node->stmtList.stmtList, level + 1);
        break;
    case NodeType_AssignStmt:
        printf("Assign: %s =  (line %d)\n", node->assignStmt.varName, node->lineno);
        traverseAST(node->assignStmt.expr, level + 1);
        break;
    case NodeType_BinOp:
        printf("BinOp: %s (line %d)\n", node->binOp.operator, node->lineno);
        traverseAST(node->binOp.left, level + 1);
        traverseAST(node->binOp.right, level + 1);
        break;
    case NodeType_FunctionDecl:
        printf("FunctionDecl: %s (line %d)\n", node->funcDecl.funcName, node->lineno);
        traverseAST(node->funcDecl.paramList, level + 1);
        traverseAST(node->funcDecl.funcBody, level + 1);
        break;
    case NodeType_ParamList:
        traverseAST(node->paramList.param, level + 1);
        traverseAST(node->paramList.paramList, level + 1);
        break;
    case NodeType_Param:
        printf("Param: %s %s (line %d)\n", node->param.paramType, node->param.paramName, node->lineno);
        break;
    case NodeType_FunctionCall:
        printf("FunctionCall: %s (line %d)\n", node->funcCall.funcName, node->lineno);
        traverseAST(node->funcCall.argList, level + 1);
        break;
    case NodeType_ArgList:
        traverseAST(node->argList.arg, level + 1);
        traverseAST(node->argList.argList, level + 1);
        break;
    case NodeType_Arg:
        traverseAST(node->arg.arg, level + 1);
        break;
    case NodeType_ArrayDecl:
        printf("ArrayDecl: %s (line %d)\n", node->arrayDecl.arrayName, node->lineno);
        printf("Array Size: %d\n", node->arrayDecl.sizeExpr);
        // TODO Traverse the Array
        break;
    case NodeType_ArrayAccess:
        printf("ArrayAccess: %s (line %d)\n", node->arrayAccess.arrayName, node->lineno);
        // printf("Array Index: %d\n", node->arrayAccess.indexExpr);
        // TODO Traverse the Array
        break;
    case NodeType_WriteStmt:
        printf("Write (line %d)\n", node->lineno);
        traverseAST(node->writeStmt.expr, level + 1);
        break;
    }
}

void freeAST(ASTNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
    case NodeType_SimpleExpr:
        // No dynamic memory to free for simple expressions
        break;

    case NodeType_SimpleID:
        // No dynamic memory to free for simple identifiers
        break;
    case NodeType_Program:
        freeAST(node->program.varDeclList);
        freeAST(node->program.stmtList);
        break;
    case NodeType_VarDeclList:
        freeAST(node->varDeclList.varDecl);
        freeAST(node->varDeclList.varDeclList);
        break;
    case NodeType_VarDecl:
        free(node->varDecl.varType);
        free(node->varDecl.varName);
        break;
    case NodeType_Expr:
        freeAST(node->expr.left);
        freeAST(node->expr.right);
        break;
    case NodeType_StmtList:
        freeAST(node->stmtList.stmt);
        freeAST(node->stmtList.stmtList);
        break;
    case NodeType_AssignStmt:
        free(node->assignStmt.varName);
        freeAST(node->assignStmt.expr);
        break;
    case NodeType_BinOp:
        freeAST(node->binOp.left);
        freeAST(node->binOp.right);
        break;
    case NodeType_FunctionDecl:
        free(node->funcDecl.funcName);
        free(node->funcDecl.returnType);
        freeAST(node->funcDecl.paramList);
        freeAST(node->funcDecl.funcBody);
        break;
    case NodeType_ParamList:
        freeAST(node->paramList.param);
        freeAST(node->paramList.paramList);
        break;
    case NodeType_Param:
        free(node->param.paramType);
        free(node->param.paramName);
        break;
    case NodeType_FunctionCall:
        free(node->funcCall.funcName);
        freeAST(node->funcCall.argList);
        break;
    case NodeType_ArgList:
        freeAST(node->argList.arg);
        freeAST(node->argList.argList);
        break;
    case NodeType_Arg:
        freeAST(node->arg.arg);
        break;
    case NodeType_ArrayDecl:
        free(node->arrayDecl.arrayName);
        free(node->arrayDecl.arrayType);
        break;
    case NodeType_ArrayAccess:
        free(node->arrayAccess.arrayName);
        break;
    case NodeType_WriteStmt:
        freeAST(node->writeStmt.expr);
        break;
    }

    free(node);
}

void printBranches(int level)
{
    for (int i = 0; i < level; i++)
    {
        if (i == level - 1)
        {
            printf("+-"); // Change: Mark the last branch
        }
        else
        {
            printf("| ");
        }
    }
}

ASTNode *createNode(NodeType type)
{
    ASTNode *newNode = (ASTNode *)malloc(sizeof(ASTNode));
    if (newNode == NULL)
    {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    newNode->type = type;
    newNode->lineno = 0;

    switch (type)
    {
    case NodeType_Program:
        printf("Creating Program Node\n");
        newNode->program.varDeclList = NULL;
        newNode->program.stmtList = NULL;
        break;
    case NodeType_VarDeclList:
        printf("Creating Variable Declaration List Node\n");
        newNode->varDeclList.varDecl = NULL;
        newNode->varDeclList.varDeclList = NULL;
        break;
    case NodeType_StmtList:
        printf("Creating Statement List Node\n");
        newNode->stmtList.stmt = NULL;
        newNode->stmtList.stmtList = NULL;
        break;
    case NodeType_SimpleExpr:
        printf("Creating Simple Expression Node\n");
        break;
    case NodeType_Expr:
        printf("Creating Expression Node\n");
        newNode->expr.left = NULL;
        newNode->expr.right = NULL;
        break;
    case NodeType_SimpleID:
        printf("Creating Simple ID Node\n");
        break;
    case NodeType_AssignStmt:
        printf("Creating Assignment Statement Node\n");
        newNode->assignStmt.varName = NULL;
        newNode->assignStmt.expr = NULL;
        break;
    case NodeType_BinOp:
        printf("Creating Binary Operation Node\n");
        newNode->binOp.left = NULL;
        newNode->binOp.right = NULL;
        break;
    case NodeType_VarDecl:
        printf("Creating Variable Declaration Node\n");
        newNode->varDecl.varType = NULL;
        newNode->varDecl.varName = NULL;
        break;
    case NodeType_FunctionDecl:
        printf("Creating Function Declaration Node\n");
        newNode->funcDecl.funcName = NULL;
        newNode->funcDecl.returnType = NULL;
        newNode->funcDecl.paramList = NULL;
        newNode->funcDecl.funcBody = NULL;
        break;
    case NodeType_ParamList:
        printf("Creating Parameter List Node\n");
        newNode->paramList.param = NULL;
        newNode->paramList.paramList = NULL;
        break;
    case NodeType_Param:
        printf("Creating Parameter Node\n");
        newNode->param.paramType = NULL;
        newNode->param.paramName = NULL;
        break;
    case NodeType_FunctionCall:
        printf("Creating Function Call Node\n");
        newNode->funcCall.funcName = NULL;
        newNode->funcCall.argList = NULL;
        break;
    case NodeType_ArgList:
        printf("Creating Argument List Node\n");
        newNode->argList.arg = NULL;
        newNode->argList.argList = NULL;
        break;
    case NodeType_Arg:
        printf("Creating Argument Node\n");
        newNode->arg.arg = NULL;
        break;
    case NodeType_ArrayDecl:
        printf("Creating Array Declaration Node\n");
        newNode->arrayDecl.arrayName = NULL;
        newNode->arrayDecl.arrayType = NULL;
        newNode->arrayDecl.sizeExpr = -1;
        break;
    case NodeType_ArrayAccess:
        printf("Creating Array Access Node\n");
        newNode->arrayAccess.arrayName = NULL;
        newNode->arrayAccess.indexExpr = NULL;
        break;
    case NodeType_WriteStmt:
        printf("Creating Write Statement Node\n");
        newNode->writeStmt.expr = NULL;
        break;
    default:
        printf("Unknown NodeType in createNode\n");
        break;
    }

    return newNode;
}
