#!/bin/bash

set -e

INPUT_FILE=${1:-ejemplo.txt}

echo "1. Generando parser con Bison..."
bison -d calc-sintaxis.y

echo "2. Generando scanner con Flex..."
flex our_flex.l

echo "3. Compilando el proyecto con GCC..."
gcc -o compilador ast.c symtab.c typecheck.c interpreter.c calc-sintaxis.tab.c lex.yy.c -lfl

echo "¡Compilación exitosa!"
echo ""

echo "Ejecutando el compilador con el archivo: $INPUT_FILE"
echo "----------------------------------------------------"
./compilador "$INPUT_FILE"
echo "----------------------------------------------------"
echo "Ejecución finalizada."