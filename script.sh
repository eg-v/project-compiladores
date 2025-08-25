#!/bin/bash

bison -d calc-sintaxis.y
flex our_flex.l
gcc -o compilador calc-sintaxis.tab.c lex.yy.c  ast.c -lflD

