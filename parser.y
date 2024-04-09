%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AST.h"
#include "symbolTable.h"
#include "semantic.h"
#include "codeGenerator.h"
#include "optimizer.h"
#include "tac.h"

#define TABLE_SIZE 100

extern int yylex();   // Declare yylex, the lexer function
extern int yyparse(); // Declare yyparse, the parser function
extern FILE* yyin;    // Declare yyin, the file pointer for the input file
extern int yylineno;  // Declare yylineno, the line number counter
extern TAC* tacHead;  // Declare the head of the linked list of TAC entries

void yyerror(const char* s);

ASTNode* root = NULL; 
SymbolTable* symTab = NULL;
Symbol* symbol = NULL;

%}

%union {
    int number;
    char character;
    char* string;
    char* operator;
    struct ASTNode* ast;
}

%token <string> TYPE
%token <string> ID
%token SEMICOLON
%token <operator> EQ
%token <operator> PLUS
%token <number> NUMBER
%token <string> WRITE
%token <string> LBRACKET
%token <string> RBRACKET
%token <string> LPAREN
%token <string> RPAREN

%printer { fprintf(yyoutput, "%s", $$); } ID;

%type <ast> Program VarDecl VarDeclList Stmt StmtList Expr BinOp FuncDecl FuncCall
%start Program

%%

Program: VarDeclList StmtList {
    printf("The PARSER has started\n");
    root = malloc(sizeof(ASTNode));
    root->type = NodeType_Program;
    root->program.varDeclList = $1;
    root->program.stmtList = $2;
}

VarDeclList:  { }
    | VarDecl VarDeclList {
        printf("PARSER: Recognized variable declaration list\n");

        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_VarDeclList;
        $$->varDeclList.varDecl = $1;
        $$->varDeclList.varDeclList = $2;
    }
;

VarDecl: TYPE ID SEMICOLON { 
            printf("PARSER: Recognized variable declaration: %s\n", $2);

            $$ = malloc(sizeof(ASTNode));
            $$->type = NodeType_VarDecl;
            $$->varDecl.varType = strdup($1);
            $$->varDecl.varName = strdup($2);

        }
        | TYPE ID LBRACKET NUMBER RBRACKET SEMICOLON { 
            printf("PARSER: Recognized array declaration: %s[%d]\n", $2, $4);

            $$ = malloc(sizeof(ASTNode));
            $$->type = NodeType_ArrayDecl;
            $$->arrayDecl.arrayType = strdup($1);
            $$->arrayDecl.arrayName = strdup($2);
            $$->arrayDecl.sizeExpr = $4;  

            if ($4 <= 0) {
                printf("Error: Array size must be a positive integer.\n");
                exit(0);
            } 
        }
        | FuncDecl SEMICOLON {  } 
;


FuncDecl: TYPE ID LPAREN VarDeclList RPAREN StmtList {
    printf("PARSER: Recognized function declaration: %s\n", $2);
    enterScope();

    $$ = malloc(sizeof(ASTNode));
    $$->type = NodeType_FunctionDecl;
    $$->funcDecl.returnType = strdup($1); 
    $$->funcDecl.funcName = strdup($2);
    $$->funcDecl.paramList = $4; 
    $$->funcDecl.funcBody = $6; 

    exitScope();
}

FuncCall: ID LPAREN RPAREN {
    printf("PARSER: Recognized function call: %s()\n", $1);

    $$ = malloc(sizeof(ASTNode));
    $$->type = NodeType_FunctionCall;
    $$->funcCall.funcName = strdup($1);

}
    | ID LPAREN Expr RPAREN {
        printf("PARSER: Recognized function call with arguments: %s()\n", $1);

        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_FunctionCall;
        $$->funcCall.funcName = strdup($1);
        $$->funcCall.argList = $3; 
    }
;

StmtList:  {}
    | Stmt StmtList {
        printf("PARSER: Recognized statement list\n");
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_StmtList;
        $$->stmtList.stmt = $1;
        $$->stmtList.stmtList = $2;
    }
;

Stmt: ID EQ Expr SEMICOLON {
    printf("PARSER: Recognized assignment statement\n");
    $$ = malloc(sizeof(ASTNode));
    $$->type = NodeType_AssignStmt;
    $$->assignStmt.varName = strdup($1);
    $$->assignStmt.operator = $2;
    $$->assignStmt.expr = $3;
}
    | WRITE Expr SEMICOLON {
        printf("PARSER: Recognized write statement\n");
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_WriteStmt;
        $$->writeStmt.expr = $2;
    }
;

Expr: Expr BinOp Expr {
    printf("PARSER: Recognized expression\n");
    $$ = malloc(sizeof(ASTNode));
    $$->type = NodeType_Expr;
    $$->expr.left = $1;
    $$->expr.right = $3;
    $$->expr.operator = $2->binOp.operator;

}
    | ID {
        printf("ASSIGNMENT statement \n");
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleID;
        $$->simpleID.name = $1;
    }
    | NUMBER {
        printf("PARSER: Recognized number\n");
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_SimpleExpr;
        $$->simpleExpr.number = $1;
    }
    | FuncCall {
        $$ = $1;
    }
    | ID LBRACKET Expr RBRACKET {
        // Create AST node for Array access
        $$ = malloc(sizeof(ASTNode));
        $$->type = NodeType_ArrayAccess;
        $$->arrayAccess.arrayName = strdup($1);
        $$->arrayAccess.indexExpr = $3;
    }
    | LPAREN Expr RPAREN {
        // TODO Add handle for parameters
    }
;

BinOp: PLUS {
    printf("PARSER: Recognized binary operator\n");
    $$ = malloc(sizeof(ASTNode));
    $$->type = NodeType_BinOp;
    $$->binOp.operator = $1;
}
;

%%
int main() {
    int semanticErrors = 0;
    // Initialize file or input source
    yyin = fopen("testProg.cmm", "r");

    // Initialize symbol table
    symTab = createSymbolTable(TABLE_SIZE);
    if (symTab == NULL) {
        fprintf(stderr, "Failed to create symbol table\n");
        return EXIT_FAILURE;
    }

    // Initialize temporary variables for TAC generation
    initializeTempVars();

    // Start parsing
    if (yyparse() == 0) {
        printf("Parsing completed successfully.\n");

        // Traverse AST for debugging
        printf("\n+++ AST Traversal +++\n");
        traverseAST(root, 0); // This prints the AST for debugging
        printf("\n+++++++++++++++++++++\n");

        // Semantic Analysis
        printf("\n--- Semantic Analysis ---\n");
        semanticErrors = semanticAnalysis(root, symTab); // Perform full semantic analysis
        printf("\n------------------------\n");

        // Check if semantic analysis was successful
        if (semanticErrors == 0) {
            // Print symbol table for debugging
            printf("\n*** Symbol Table ***\n");
            printSymbolTable(symTab);
            printf("\n********************\n");

            // TAC Generation
            printf("\n$$$ TAC Generation $$$\n");
            ASTtoTAC(root); // Changed from generateTACForExpr to ASTtoTAC
            printTACToFile("TAC.ir", tacHead); // Print the generated TAC

            // Code Optimization (If you have this phase implemented)
            optimizeTAC(&tacHead); 
            printOptimizedTAC("TACOptimized.ir", tacHead);

            // MIPS Code Generation
            printf("\n=== MIPS Code Generation ===\n");
            initCodeGenerator("Output.s", symTab); // Initialize code generation
            generateMIPS(tacHead); // Generate MIPS code from TAC
            finalizeCodeGenerator("Output.s"); // Finalize code generation and write to file

        } else {
            fprintf(stderr, "Compilation stopped due to semantic errors.\n");
        }

        // Cleanup
        freeAST(root);
        freeSymbolTable(symTab);

    } else {
        fprintf(stderr, "Parsing failed\n");
    }

    fclose(yyin);
    return 0;
}


void yyerror(const char* s) {
	fprintf(stderr, "Parse error: %s\n", s);
	exit(1);
}
