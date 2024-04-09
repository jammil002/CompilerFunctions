#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tac.h"

typedef enum
{
    NodeType_Program,
    NodeType_VarDeclList,
    NodeType_VarDecl,
    NodeType_SimpleExpr,
    NodeType_SimpleID,
    NodeType_Expr,
    NodeType_StmtList,
    NodeType_AssignStmt,
    NodeType_WriteStmt,
    NodeType_BinOp,
    NodeType_FunctionDecl,
    NodeType_ParamList,
    NodeType_Param,
    NodeType_FunctionCall,
    NodeType_ArgList,
    NodeType_Arg,
    NodeType_ArrayDecl,
    NodeType_ArrayAccess
} NodeType;

typedef struct ASTNode
{
    NodeType type;
    int lineno;
    union
    {
        struct
        {
            struct ASTNode *varDeclList;
            struct ASTNode *stmtList;
        } program;

        struct
        {
            struct ASTNode *varDecl;
            struct ASTNode *varDeclList;
        } varDeclList;

        struct
        {
            char *varType;
            char *varName;
        } varDecl;

        struct
        {
            int number;
        } simpleExpr;

        struct
        {
            char *name;
        } simpleID;

        struct
        {
            char *operator;
            struct ASTNode *left;  // Left operand
            struct ASTNode *right; // Right operand
        } expr;

        struct
        {
            struct ASTNode *stmt;
            struct ASTNode *stmtList;
        } stmtList;

        struct
        {
            char *operator;
            char *varName;
            struct ASTNode *expr;
        } assignStmt;

        struct
        {
            char *operator;
            struct ASTNode *left;
            struct ASTNode *right;
        } binOp;

        struct
        {
            char *funcName;
            char *returnType;
            struct ASTNode *paramList;
            struct ASTNode *funcBody;
        } funcDecl;

        struct
        {
            struct ASTNode *param;
            struct ASTNode *paramList;
        } paramList;

        struct
        {
            char *paramType;
            char *paramName;
        } param;

        struct
        {
            char *funcName;
            struct ASTNode *argList;
        } funcCall;

        struct
        {
            struct ASTNode *arg;
            struct ASTNode *argList;
        } argList;

        struct
        {
            struct ASTNode *arg;
        } arg;

        struct
        {
            char *arrayName;
            char *arrayType;
            int sizeExpr;
        } arrayDecl;

        struct
        {
            struct ASTNode *indexExpr;
            char *arrayName;
        } arrayAccess;

        struct
        {
            struct ASTNode *expr;
        } writeStmt;
    };
} ASTNode;

void traverseAST(ASTNode *node, int level);
ASTNode *createNode(NodeType type);
void printBranches(int level);
void ASTtoTAC(ASTNode *root);
void freeAST(ASTNode *node);

#endif // AST_H