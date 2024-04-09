#include "../AST.h"
#include <stdio.h>

int main()
{
    struct AST *typeDecl = createASTNode(NodeType_Type, "Type", "int", "x", NULL, NULL);
    struct AST *assignment = createASTNode(NodeType_Assignment, "Assignment", "x", "10", NULL, NULL);
    struct AST *writeStmt = createASTNode(NodeType_Write, "Write", "x", "", NULL, NULL);

    printf("AST for type declaration:\n");
    printAST(typeDecl, 0);

    printf("\nAST for assignment:\n");
    printAST(assignment, 0);

    printf("\nAST for write statement:\n");
    printAST(writeStmt, 0);

    freeAST(typeDecl);
    freeAST(assignment);
    freeAST(writeStmt);

    return 0;
}
