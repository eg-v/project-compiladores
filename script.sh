#!/bin/bash

flex our_flex.l
bison -d calc-sintaxis.y
gcc lex.yy.c calc-sintaxis.tab.c -Wno-error=implicit-function-declaration
