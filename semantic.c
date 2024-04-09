#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tempVars.h"
#include "symbolTable.h"
#include "tac.h"
#define TABLE_SIZE 100

int semanticAnalysis(ASTNode *node, SymbolTable *symTab)
{
    Symbol *symbol;
    TAC *tac;
    int semanticErrors = 0;

    if (node == NULL)
        return 1; // Early return for a null node

    switch (node->type)
    {
    case NodeType_Program:
        printf("Analyzing Program\n");
        semanticAnalysis(node->program.varDeclList, symTab);
        semanticAnalysis(node->program.stmtList, symTab);
        break;

    case NodeType_VarDeclList:
        printf("Analyzing Variable Declaration List\n");
        semanticAnalysis(node->varDeclList.varDecl, symTab);
        semanticAnalysis(node->varDeclList.varDeclList, symTab);
        break;

    case NodeType_VarDecl:
        printf("Analyzing Variable Declaration\n");
        symbol = lookupSymbol(symTab, node->varDecl.varName);
        if (symbol != NULL)
        {
            fprintf(stderr, "Semantic error: Variable %s redeclared at line %d\n", node->varDecl.varName, node->lineno);
            semanticErrors++;
        }
        else
        {
            addSymbol(symTab, node->varDecl.varName, node->varDecl.varType);
        }
        break;

    case NodeType_StmtList:
        printf("Analyzing Statement List\n");
        semanticAnalysis(node->stmtList.stmt, symTab);
        semanticAnalysis(node->stmtList.stmtList, symTab);
        break;

    case NodeType_AssignStmt:
        printf("Analyzing Assignment Statement\n");
        semanticAnalysis(node->assignStmt.expr, symTab);
        symbol = lookupSymbol(symTab, node->assignStmt.varName);
        if (symbol == NULL)
        {
            fprintf(stderr, "Semantic error: Variable %s used without declaration at line %d\n", node->assignStmt.varName, node->lineno);
            semanticErrors++;
        }
        break;

    case NodeType_Expr:
        printf("Analyzing Expression\n");
        semanticAnalysis(node->expr.left, symTab);
        semanticAnalysis(node->expr.right, symTab);
        break;

    case NodeType_BinOp:
        printf("Analyzing Binary Operation\n");
        semanticAnalysis(node->binOp.left, symTab);
        semanticAnalysis(node->binOp.right, symTab);
        break;

    case NodeType_SimpleID:
        printf("Analyzing Simple ID\n");
        if (lookupSymbol(symTab, node->simpleID.name) == NULL)
        {
            fprintf(stderr, "Semantic error: Variable %s has not been declared at line %d\n", node->simpleID.name, node->lineno);
            semanticErrors++;
        }
        break;

    case NodeType_SimpleExpr:
        printf("Analyzing Simple Expression\n");
        // Typically, there's no semantic error possible here for just a number.
        break;

    case NodeType_FunctionDecl:
        printf("Analyzing Function Declaration\n");
        symbol = lookupSymbol(symTab, node->funcDecl.funcName);
        if (symbol != NULL)
        {
            fprintf(stderr, "Semantic error: Function %s redeclared at line %d\n", node->funcDecl.funcName, node->lineno);
            semanticErrors++;
        }
        else
        {
            addSymbol(symTab, node->funcDecl.funcName, "function");
            SymbolTable *funcSymTab = createSymbolTable(TABLE_SIZE);
            semanticAnalysis(node->funcDecl.paramList, funcSymTab);
            semanticAnalysis(node->funcDecl.funcBody, funcSymTab);
        }
        break;

    case NodeType_ParamList:
        printf("Analyzing Parameter List\n");
        semanticAnalysis(node->paramList.param, symTab);
        semanticAnalysis(node->paramList.paramList, symTab);
        break;

    case NodeType_Param:
        printf("Analyzing Parameter\n");
        symbol = lookupSymbol(symTab, node->param.paramName);
        if (symbol != NULL)
        {
            fprintf(stderr, "Semantic error: Parameter %s redeclared at line %d\n", node->param.paramName, node->lineno);
            semanticErrors++;
        }
        else
        {
            addSymbol(symTab, node->param.paramName, node->param.paramType);
        }
        break;

    case NodeType_FunctionCall:
        printf("Analyzing Function Call\n");
        symbol = lookupSymbol(symTab, node->funcCall.funcName);
        if (symbol == NULL || !symbol->isFunction)
        {
            fprintf(stderr, "Semantic error: Function %s called without declaration at line %d\n", node->funcCall.funcName, node->lineno);
            semanticErrors++;
        }
        else
        {
            ASTNode *arg = node->funcCall.argList;
            while (arg != NULL)
            {
                semanticAnalysis(arg->arg.arg, symTab);
                arg = arg->argList.argList;
            }
        }
        break;

    case NodeType_ArgList:
        printf("Analyzing Argument List\n");
        semanticAnalysis(node->argList.arg, symTab);
        semanticAnalysis(node->argList.argList, symTab);
        break;

    case NodeType_Arg:
        printf("Analyzing Argument\n");
        semanticAnalysis(node->arg.arg, symTab);
        break;

    case NodeType_ArrayDecl:
        printf("Analyzing Array Declaration\n");
        symbol = lookupSymbol(symTab, node->arrayDecl.arrayName);
        if (symbol != NULL)
        {
            fprintf(stderr, "Semantic error: Array %s redeclared at line %d\n", node->arrayDecl.arrayName, node->lineno);
            semanticErrors++;
        }
        else
        {
            addSymbol(symTab, node->arrayDecl.arrayName, node->arrayDecl.arrayType);
        }
        break;

    case NodeType_ArrayAccess:
        printf("Analyzing Array Access\n");
        symbol = lookupSymbol(symTab, node->arrayAccess.arrayName);
        if (symbol == NULL || !symbol->isArray)
        {
            fprintf(stderr, "Semantic error: Array %s accessed without declaration at line %d\n", node->arrayAccess.arrayName, node->lineno);
            semanticErrors++;
        }
        else
        {
            semanticAnalysis(node->arrayAccess.indexExpr, symTab);
        }
        break;

    case NodeType_WriteStmt:
        printf("Analyzing Write Statement\n");
        semanticAnalysis(node->writeStmt.expr, symTab);
        break;

    default:
        fprintf(stderr, "Unknown Node Type: %u\n", node->type);
        semanticErrors++;
        break;
    }

    return semanticErrors;
}
