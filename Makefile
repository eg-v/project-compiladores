CC = gcc
CFLAGS = -Wno-error=implicit-function-declaration

all: lex.yy.c calc-sintaxis.tab.c executable

lex.yy.c: our_flex.l
	flex our_flex.l

calc-sintaxis.tab.c calc-sintaxis.tab.h: calc-sintaxis.y
	bison -d calc-sintaxis.y

executable: lex.yy.c calc-sintaxis.tab.c ast.c typecheck.c symtab.c interpreter.c assembler.c
	$(CC) lex.yy.c calc-sintaxis.tab.c ast.c typecheck.c symtab.c interpreter.c assembler.c $(CFLAGS) -o executable

clean:
	rm -f lex.yy.c calc-sintaxis.tab.c calc-sintaxis.tab.h executable
