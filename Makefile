all: parser

parser.tab.c parser.tab.h:	parser.y
	bison -t -v -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

parser: lex.yy.c parser.tab.c parser.tab.h AST.c symbolTable.c semantic.c codeGenerator.c optimizer.c tac.c
	gcc -o parser parser.tab.c lex.yy.c AST.c symbolTable.c semantic.c codeGenerator.c optimizer.c tac.c
	./parser testProg.cmm

clean:
	rm -f parser parser.tab.c lex.yy.c parser.tab.h parser.output lex.yy.o parser.tab.o AST.o semantic.o symbolTable.o codeGenerator.o optimizer.o tac.o TAC.ir TACoptimized.ir Output.s
	ls -l
